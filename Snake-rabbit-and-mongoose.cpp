#include<stdio.h>
#include<Windows.h>
#include <conio.h>
#include<math.h>
#include<time.h>
#include<vector>
using namespace std;
const int C = 30, D0 = 80;

struct Message{
	/*��������� ������������� ��� ������� ������ ���� ��� ������� �������� ������ ����*/
	
	int code, x, y, vx, vy;
	bool answer;
} message;

class Animal{
protected:
	int x, y, color, id;
public:
	Animal(int x, int y, int color){
		this->x = x;
		this->y = y;
		this->color = color;
		this->id = 0;
	}
	virtual void draw(HDC hdc){
		/*��������� ���������
			����� ���������: �������*/
		RECT r;
		r.left = x*C+D0;
		r.top = y*C+D0;
		r.right = (x+1)*C+D0;
		r.bottom = (y+1)*C+D0;
		HBRUSH hBrush = CreateSolidBrush(color);
		FillRect(hdc, &r, hBrush);
		DeleteObject(hBrush);
	}
	virtual int get_x(){
		return x;
	}
	virtual int get_y(){
		return y;
	}
	virtual int get_color(){
		return color;
	}
	virtual int get_id(){
		return id;
	}
	virtual void new_coord(int x, int y){
		this->x = x;
		this->y = y;
	}
};

class Mangust: public Animal{
public:
	Mangust(int x, int y, int color):Animal(x,y,color){
		this->id = 1;
	}
};

class Rabbit: public Animal{
public:
	Rabbit(int x, int y, int color):Animal(x,y,color){
		this->id = 2;
	}
	void draw(HDC hdc){
		/*��������������� ������ draw
			����� �������: ����*/

		HPEN hpen = CreatePen(PS_SOLID, 1, color);
		HBRUSH hBrush = CreateSolidBrush(color);
		SelectObject(hdc, hpen);
		SelectObject(hdc, hBrush);
		Ellipse(hdc, x*C+D0, y*C+D0, (x+1)*C+D0, (y+1)*C+D0);
		DeleteObject(hBrush);
		DeleteObject(hpen);
	}
};

class Snake_piece: public Animal{
	public:
	Snake_piece(int x, int y, int color):Animal(x,y,color){}
};

class Snake{
protected:
	int len, vx, vy;
	vector <Snake_piece*> piece;
public:
	Snake(int len){
		this->len = len;
		this->vx = 1;
		this->vy = 0;
		for (int i=0;i<len;i++){
			piece.insert(piece.begin(), new Snake_piece(2+i,4,RGB(0,150,0)));
		}
	}
	void draw(HDC hdc){
		/*����� ������������ ��� ��������� ���� ����� ��������� ������� ������� ����*/

		for (int i = 0; i < len; i++){
			piece[i]->draw(hdc);
		}
	}
	void move(HDC hdc, bool grow){
		/*����� ������������ ��� ������������ ����
			���� ���������� ������ "�������" � ������ ���� � �������� ���������� �������;
			���� ���� �����, �� ��������� "�������" �� ���������*/

		piece.insert(piece.begin(), new Snake_piece(piece[0]->get_x() + vx, piece[0]->get_y() + vy,  piece[0]->get_color()));
		if (grow) this->len++;
		else {
			delete piece[len];
			piece.pop_back();
		}
		this->draw(hdc);
	}
	void change_direction(int new_vx, int new_vy){
		/*����� ������������ ��� ��������� ����������� �������� ����,
		���� ��� �������� - ���� ����� �������� ����������� ������ �� 90 ��������, 
		�.�.  ������ ��������� ��������� ��� ���� (������->������) � (������->�����)*/
		
		if (vx * new_vx == vy * new_vy){
			vx = new_vx;
			vy = new_vy; 
		} 
	}
	bool check_self_eat(){
		/*�������� ���������������*/

		int x = piece[0]->get_x(), y = piece[0]->get_y(); 
		for (int i = 1; i < len; i++){
			if(x == piece[i]->get_x() && y == piece[i]->get_y()) return true;
		}
		return false;
	}
	bool check_new_coord(int new_x, int new_y){
		/*����� ��������� �������� ���������� ����� ��������� ��������� �
		��������� �������� ����*/

		for (int i = 0; i < len; i++)
			if(new_x == piece[i]->get_x() && new_y == piece[i]->get_y()) return false;
		return true;
	}
	void Event(Message *mes){
		/*����� ������������ ��� ��������� �������� ������ ��� ������� �������� ������ ����*/

		switch (mes->code){
			case 77: mes->answer = this->check_self_eat();
			mes->x = piece[0]->get_x(); mes->y = piece[0]->get_y(); 
			mes->vx = this->vx; mes->vy = this->vy;	break;
			
			case 88: mes->x = piece[0]->get_x(); mes->y = piece[0]->get_y(); 
			mes->vx = this->vx; mes->vy = this->vy;	break;
			
			case 100: mes->answer = this->check_new_coord(mes->x, mes->y);
		}
	}
};

class Field{
protected:
	int height, width, speed, time_to_change_mang;
	Snake * snake;
	Mangust * mang;
	Rabbit * rabbit;
	Animal * rand_animal;
public:
	Field(int height, int width, int speed, int count){
	this->speed = speed;
	this->time_to_change_mang = count;
	this->height = height;
	this->width = width;
	Message message = {0, 0, 0, 0, 0, true};
	init_animals();
}
	void init_animals(){
		/*����� ������������ ��� ������������� �������� 
		�� ���������� ������������ � ������������ 
		������ � ������ �������� ����*/

		snake = new Snake(4);
		srand(time(NULL));
		int  y1 = 3 + int(floor((height - 4 - 1)/2)), y2 =  y1 + int(floor((height - 5) / 2));
		int mang_x = 1 + rand() % (width  - 2), rabbit_x = 1 + rand() % (width - 2);
		
		mang = new Mangust(mang_x, y1, RGB(200,0,0));		
		rabbit = new Rabbit(rabbit_x, y2, RGB(255,255,255));

		init_random_animal(rabbit_x, y2-2);
	}
	void init_random_animal(int x, int y){
		/*����� ������������ ��� ������������� ��������� ��������� �������*/

		srand(time(NULL));
		switch(rand() % 2){
			case 0: rand_animal = new Mangust(x, y, RGB(200,0,0)); break;
			case 1: rand_animal = new Rabbit(x, y, RGB(255,255,255)); break;
		}
	}
	void draw(HDC hdc){
		/*����� ������������ ��� ��������� ���� 
			����� ��������� 2 ��������� ������� ������� � �����*/

	RECT r;
	r.left = D0;
	r.top = D0;
	r.right = width*C+D0;
	r.bottom = height*C+D0;
	HBRUSH hBrush = CreateSolidBrush(RGB(139,69,19));
	FillRect(hdc, &r, hBrush);
	hBrush = CreateSolidBrush(RGB(200, 200, 200));
	r.left = D0+C;
	r.top = D0+C;
	r.right = (width-1)*C+D0;
	r.bottom = (height-1)*C+D0;
	FillRect(hdc, &r, hBrush);
	DeleteObject(hBrush);
	DeleteObject(hBrush);
}
	bool check_mang_position(int head_x, int head_y, int vx, int vy, int mang_x, int mang_y){ 
		/*�������� ������������ ������ ���� � ��������� 
		�� ��������� �������� ��������� �����*/
		
		switch(vx){
		case 1: if(head_x == mang_x - 1 && head_y == mang_y) return true;break;
		case -1: if(head_x == mang_x + 1 && head_y == mang_y) return true;break;
		case 0:
			{
				switch(vy){
					case 1: if(head_x == mang_x && head_y == mang_y - 1) return true;break;
					case -1: if(head_x == mang_x && head_y == mang_y + 1) return true;break;
				}
			}break;
		}
		return false;
	}
	bool check_wall(int head_x, int head_y, int vx, int vy){
		/*�������� ������������ ������ ���� �� ������ 
		�� ��������� �������� ��������� �����*/

		switch(vx){
		case 1: if(head_x == width - 2) return true;break;
		case -1: if(head_x == 1) return true;break;
		case 0:
			switch(vy){
				case 1: if(head_y == height - 2) return true;break;
				case -1: if(head_y == 1) return true;break;
			}break;
		}
		return false;
	}
	bool check_death(bool is_mangust){
		/*����� ������������ ��� �������� ������� ��������� ����*/

		bool hello_wall = false, hello_mang = false;
		message.code = 77;
		snake->Event(&message);
		
		hello_wall = check_wall(message.x, message.y, message.vx, message.vy);
		hello_mang = check_mang_position(message.x, message.y, message.vx, message.vy, mang->get_x(), mang->get_y());
		if (is_mangust)
			hello_mang = hello_mang || this->check_mang_position(message.x, message.y, message.vx, message.vy, rand_animal->get_x(), rand_animal->get_y());

		return message.answer || hello_wall || hello_mang;
	}	
	bool check_rabbit(Animal * animal){
		/*����� ������������ ��� �������� �������� ������� (������������� ����� ����)*/ 		

		message.code = 88;
		snake->Event(&message);
		if (message.x == animal->get_x() && message.y == animal->get_y()) return true;
			else return false;
	}		
	void change_coord_animal(Animal * animal, Animal * another1, Animal * another2){
		/*����� ������������ ��� ��������� ��������� ��������� animal*/

		create_new_coord(another1, another2);
		animal->new_coord(message.x, message.y);		
	}
	void create_new_coord(Animal * animal1, Animal * animal2){
		/*�������� ����� ��������� � �� ���������� � message.x � message.y*/

		int new_x = 0, new_y = 0;
		bool cont = false;
		message.code = 100;
		int animal1_x = animal1->get_x(), animal2_x = animal2->get_x(), 
			animal1_y = animal1->get_y(), animal2_y = animal2->get_y();
				
		do{
			cont = false;
			srand(time(NULL));			
			new_x = 1 + rand() % (width - 2);
			new_y = 1 + rand() % (height - 2);
			if (animal1_x == new_x && animal1_y == new_y || animal2_x == new_x && animal2_y == new_y) cont = true; 
			else {
				message.x = new_x;
				message.y = new_y;
				snake->Event(&message);
				cont = !message.answer;
			}
		} while(cont);
	}
	void mainloop(HDC hdc){
		/*����� ��������� �������� ���� ����*/

		int iter = 0;
		bool flag = true, snake_grow = false, is_mangust = false;

		do{
			this->draw(hdc);
			rabbit->draw(hdc);
			rand_animal->draw(hdc);
			snake->move(hdc, snake_grow);	
			mang->draw(hdc);

			if (rand_animal->get_id() == 1) is_mangust = true;
			else is_mangust = false;

			snake_grow = this->check_rabbit(rabbit);

			if (snake_grow) { 
				change_coord_animal(rabbit, mang, rand_animal);
				iter -= 5;
			}			
			else if (is_mangust == false){
				snake_grow = this->check_rabbit(rand_animal);
				if (snake_grow) { 
					delete rand_animal; 
					create_new_coord(rabbit, mang); 
					init_random_animal(message.x, message.y);
					iter -= 5;
				}
				else iter++;
			} 
				else iter++;

			if (iter == int(time_to_change_mang/2)) {
				delete rand_animal; 
				create_new_coord(rabbit, mang); 
				init_random_animal(message.x, message.y); 
				iter += 5;
			}
			if (iter == time_to_change_mang){
				change_coord_animal(mang, rabbit, rand_animal); 
				iter = 0;
			}
			
			Sleep(speed);			

			if (kbhit())
				if (getch() == 224)
				switch (getch()){
					case 72: snake->change_direction(0, -1); break; //�����
                    case 80: snake->change_direction(0, 1); break; //����
                    case 75: snake->change_direction(-1, 0); break; //�����
                    case 77: snake->change_direction(1, 0); break; //������
					default: break;
				}
			
			flag = !this->check_death(is_mangust);
			
		}while(flag);
	}
};

int main()
{
	HWND hconwnd = GetConsoleWindow();
	HDC hdc = GetDC(hconwnd);

	Field field(16, 18, 100, 40);
	field.mainloop(hdc);

	system("pause");
	return 0;
}