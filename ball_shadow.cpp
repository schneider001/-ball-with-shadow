#include "TXlib.h"
#include <stdlib.h>
#include <string>
#include <cmath>


struct plot {
	double x, y, z;
};


class CoordSys {
	plot coords0_, coords1_;
	double scaleX_, scaleY_;
	const char* signature_;
public:
	double* to_pixels(plot coords);
	void draw_point(plot coords, COLORREF color_of_point);
	void draw_circle(plot coords, double radius, COLORREF color_of_circle);
	void draw_pixel(plot coords, COLORREF color_of_point);
	void draw_line(plot coords0, plot coords1, COLORREF color_of_line);
	void draw_window();
	void draw_grid();
	void draw_axis();
	void set_color_back();
	void write_signature();
	CoordSys(plot coords0, plot coords1, double scaleX, double scaleY, const char* signature) {
		coords0_ = coords0;
		coords1_ = coords1;
		signature_ = signature;
		scaleX_ = scaleX;
		scaleY_ = scaleY;
	}
};


class Vector {
public:
	plot coords_;
	double length();
	void turn_vector_counter_clock_wise(double degree);
	Vector normalization();
	Vector(plot coords) {
		coords_.x = coords.x;
		coords_.y = coords.y;
		coords_.z = coords.z;
	}
};


class Button {
	plot coords0_, coords1_;
	const char* name_;
public:
	void draw_button();
	bool if_button_pressed();
	Button(plot coords0, plot coords1, const char* name) {
		coords0_ = coords0;
		coords1_ = coords1;
		name_ = name;
	}
};


Vector operator + (Vector a, Vector b);
Vector operator * (double koef_of_length, Vector a);
double operator * (Vector a, Vector b);



int main() {
	txCreateWindow(1200, 700);
	CoordSys vector_space({ 0, 0, 0 }, { 1200, 700, 0 }, 1, 1, "vector space");
	vector_space.set_color_back();
	vector_space.draw_window();

	Button stop({ 1000, 600 }, { 1050, 650 }, "Stop");
	stop.draw_button();

	Vector source({ 500, 500, 100 });
	while (1) {
		source.turn_vector_counter_clock_wise(txPI / 30);
		for (double x = -350; x <= 350; x++) {
			for (double y = -600; y <= 600; y++) {
				if (stop.if_button_pressed()) {
					return 0;
				}
				Vector a({ x, y, 0 });
				double R = 100;
				if (a.length() < R) {
					a.coords_.z = sqrt(R * R - a.coords_.x * a.coords_.x - a.coords_.y * a.coords_.y);
					Vector normal_a = a.normalization();
					Vector L = source + -1 * a;
					Vector normal_L = L.normalization();
					double I = normal_a * normal_L / 2;
					if (I >= 0) {
						vector_space.draw_pixel({ a.coords_.x, a.coords_.y, a.coords_.z }, RGB(255 * I, 128 * I, 0));
					}
					else {
						vector_space.draw_pixel({ a.coords_.x, a.coords_.y, a.coords_.z }, RGB(0, 0, 0));
					}
				}
			}
		}
	}
}


double Vector::length() {
	return sqrt(coords_.x * coords_.x + coords_.y * coords_.y + coords_.z * coords_.z);
}


void Vector::turn_vector_counter_clock_wise(double degree) {
	plot new_coords = { 0, 0, 0 };
	new_coords.x = coords_.x * cos(degree) - coords_.y * sin(degree);
	new_coords.y = coords_.x * sin(degree) + coords_.y * cos(degree);
	coords_ = new_coords;
}


Vector Vector::normalization() {
	Vector temp_vector = *this;
	double length = temp_vector.length();
	return 1 / length * temp_vector;
}

double* CoordSys::to_pixels(plot coords) {
	plot start_of_coord = { coords1_.x / 2, coords1_.y / 2 };
	double* rec_coord = new double[2];
	rec_coord[0] = coords.x * scaleX_ + start_of_coord.x;
	rec_coord[1] = start_of_coord.y - coords.y * scaleY_;
	return rec_coord;
}


void CoordSys::draw_point(plot coords, COLORREF color_of_point) {
	txSetColor(TX_LIGHTGREEN);
	txSetFillColor(color_of_point);
	int point_radius = 4;
	double* rec_coord = to_pixels(coords);
	if (rec_coord[1] >= coords0_.y &&
		rec_coord[1] <= coords1_.y &&
		rec_coord[0] <= coords1_.x &&
		rec_coord[0] >= coords0_.x) {
		txCircle(rec_coord[0], rec_coord[1], point_radius);
	}
	delete[] rec_coord;
}


void CoordSys::draw_pixel(plot coords, COLORREF color_of_point) {
	txSetFillColor(color_of_point);
	double* rec_coord = to_pixels(coords);
	if (rec_coord[1] >= coords0_.y &&
		rec_coord[1] <= coords1_.y &&
		rec_coord[0] <= coords1_.x &&
		rec_coord[0] >= coords0_.x) {
		txSetPixel(rec_coord[0], rec_coord[1], color_of_point);
	}
	delete[] rec_coord;
}


void CoordSys::draw_circle(plot coords, double radius, COLORREF color_of_circle) {
	txSetColor(color_of_circle);
	txSetFillColor(TX_TRANSPARENT);
	double* rec_coord = to_pixels(coords);
	txCircle(rec_coord[0], rec_coord[1], radius);
	delete[] rec_coord;
}


void CoordSys::draw_line(plot coords0, plot coords1, COLORREF color_of_line) {
	txSetColor(color_of_line);
	double* rec_coord0 = to_pixels(coords0);
	double* rec_coord1 = to_pixels(coords1);
	if (rec_coord0[1] >= coords0_.y &&
		rec_coord0[1] <= coords1_.y &&
		rec_coord0[0] <= coords1_.x &&
		rec_coord0[0] >= coords0_.x &&
		rec_coord1[1] >= coords0_.y &&
		rec_coord1[1] <= coords1_.y &&
		rec_coord1[0] <= coords1_.x &&
		rec_coord1[0] >= coords0_.x) {
		txLine(rec_coord0[0], rec_coord0[1], rec_coord1[0], rec_coord1[1]);
	}
	txLine(rec_coord0[0], rec_coord0[1], rec_coord1[0], rec_coord1[1]);
	delete[] rec_coord0;
	delete[] rec_coord1;
}


void CoordSys::draw_window() {
	//txSetColor(TX_LIGHTGREEN);
	txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
}

void CoordSys::draw_grid() {
	for (int line = coords0_.x; line < coords1_.x; line += 10) {
		txLine(line, coords0_.y, line, coords1_.y);
	}
	for (int line = coords0_.y; line < coords1_.y; line += 10) {
		txLine(coords0_.x, line, coords1_.x, line);
	}
}


void CoordSys::draw_axis() {
	txSetColor(TX_BLACK);

	txLine(coords1_.x / 2, coords0_.y, coords1_.x / 2, coords1_.y);
	txLine(coords0_.x, coords1_.y / 2, coords1_.x, coords1_.y / 2);

	txLine(coords1_.x / 2, coords0_.y, coords1_.x / 2 + 5, coords0_.y + 5);
	txLine(coords1_.x / 2, coords0_.y, coords1_.x / 2 - 5, coords0_.y + 5);

	txLine(coords1_.x - 5, coords1_.y / 2 + 5, coords1_.x, coords1_.y / 2);
	txLine(coords1_.x - 5, coords1_.y / 2 - 5, coords1_.x, coords1_.y / 2);
}


void CoordSys::set_color_back() {
	txSetFillColor(RGB(46,40,51));
}


void CoordSys::write_signature() {
	txDrawText(coords0_.x - 30, coords1_.y + 15, coords1_.x, coords1_.y + 30, signature_);
}

void Button::draw_button() {
	txSetColor(TX_LIGHTGREEN);
	txSetFillColor(TX_BLACK);
	txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
}


bool Button::if_button_pressed() {
	if (txMouseX() >= coords0_.x &&
		txMouseX() <= coords1_.x &&
		txMouseY() >= coords0_.y &&
		txMouseY() <= coords1_.y &&
		txMouseButtons() == 1) {
		while (txMouseButtons() == 1) {
			txSleep(1);
		}
		return TRUE;
	}
	else {
		return FALSE;
	}
}


Vector operator + (Vector a, Vector b) {
	plot coords_of_new_vector = { a.coords_.x + b.coords_.x, a.coords_.y + b.coords_.y, a.coords_.z + b.coords_.z };
	return Vector(coords_of_new_vector);
}


Vector operator * (double koef_of_length, Vector a) {
	plot coords_of_new_vector = { koef_of_length * a.coords_.x, koef_of_length * a.coords_.y, koef_of_length * a.coords_.z };
	return Vector(coords_of_new_vector);
}


double operator * (Vector a, Vector b) {
	return (a.coords_.x * b.coords_.x + a.coords_.y * b.coords_.y + a.coords_.z + b.coords_.z);
}


