#include "TXlib.h"
#include <stdlib.h>
#include <string>
#include <cmath>

RGBQUAD* Video_memory = NULL;

struct plot {
	double x, y, z;
};


class Vector {
public:
	plot coords_;
	double length();
	void turn_vector_counter_clock_wise(double degree);
	Vector normalization();
	void custom_color();
	Vector(plot coords = { 0, 0, 0 }) {
		coords_.x = coords.x;
		coords_.y = coords.y;
		coords_.z = coords.z;
	}
};


class CoordSys {
	plot coords0_, coords1_;
	double scaleX_, scaleY_;
	const char* signature_;
public:
	plot to_pixels(plot coords);
	void draw_point(plot coords, Vector color_of_point, double R);
	void draw_pixel(plot coords, plot color_of_point, double size_of_window);
	void draw_window();
	void set_color_back(Vector background);
	CoordSys(plot coords0, plot coords1, double scaleX, double scaleY, const char* signature) {
		coords0_ = coords0;
		coords1_ = coords1;
		signature_ = signature;
		scaleX_ = scaleX;
		scaleY_ = scaleY;
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


class Source {
public:
	Vector position_;
	Vector color_;
	Vector lambert_speck(Vector point_on_surface, Vector camera, Vector color_of_ball, Vector default_light, double degree);
	Source(Vector position, Vector color) {
		position_ = position;
		color_ = color;
	}
};

Vector operator + (Vector a, Vector b);
Vector operator * (double koef_of_length, Vector a);
double operator * (Vector a, Vector b);
Vector comp_mult(Vector a, Vector b);


int main() {
	double size_of_window = 1000;
	txCreateWindow(size_of_window, size_of_window);
	txBegin();
	Video_memory = txVideoMemory();

	Vector background({ 0, 0, 0 });
	CoordSys vector_space({ 0, 0, 0 }, { size_of_window, size_of_window, 0 }, 1, 1, "vector space");
	vector_space.set_color_back(background);
	vector_space.draw_window();

	Button stop({ size_of_window - 150, size_of_window - 150 }, { size_of_window - 100, size_of_window - 100 }, "Stop");
	stop.draw_button();

	//Vector background({ 46, 40, 51 });
	Vector camera({ 0, 0, 500 });
	Vector color_of_ball({ 128, 128, 128 });
	Vector default_light({ 20, 20, 20 });
	Vector lebeg_color;

	Source lamp1(Vector({ 0, 300, 300 }), Vector({ 30, 145, 255 }));
	Source lamp2(Vector({ 0, -300, 300 }), Vector({ 255, 128, 0 }));
	Source lamp3(Vector({ -400, 0, 100 }), Vector({ 121, 7, 7 }));
	Source lamp4(Vector({ 400, 0, 100 }), Vector({ 121, 7, 7 }));
	lamp1.color_ = 1. / 255 * lamp1.color_;
	lamp2.color_ = 1. / 255 * lamp2.color_;
	lamp3.color_ = 1. / 255 * lamp3.color_;
	color_of_ball = 1. / 255 * color_of_ball;
	default_light = 1. / 255 * default_light;


	double R = 160;
	double rotation = 0;

	while (1) {
		double fps = txGetFPS();
		if (fps != 0) {
			rotation += 0.7 / fps;
		}

		vector_space.draw_point(plot({ lamp1.position_.coords_.x, lamp1.position_.coords_.y, lamp1.position_.coords_.y }), background, R);
		vector_space.draw_point(plot({ lamp2.position_.coords_.x, lamp2.position_.coords_.y, lamp2.position_.coords_.z }), background, R);
		vector_space.draw_point(plot({ lamp3.position_.coords_.x, lamp3.position_.coords_.y, lamp3.position_.coords_.z }), background, R);
		vector_space.draw_point(plot({ lamp4.position_.coords_.x, lamp4.position_.coords_.y, lamp4.position_.coords_.z }), background, R);
		lamp1.position_.coords_.x = 300 * sin(rotation);
		lamp1.position_.coords_.z = 300 * cos(rotation);
		lamp2.position_.coords_.x = -300 * sin(rotation);
		lamp2.position_.coords_.z = -300 * cos(rotation);
		lamp3.color_.coords_.x = abs(cos(rotation / 3));
		lamp3.color_.coords_.y = abs(cos(rotation / 3));
		lamp3.color_.coords_.z = abs(sin(rotation / 3));
		lamp4.color_.coords_.x = abs(cos(rotation / 3));
		lamp4.color_.coords_.y = abs(cos(rotation / 3));
		lamp4.color_.coords_.z = abs(sin(rotation / 3));

		for (double x = -R; x <= R; x++) {
			for (double y = -R; y <= R; y++) {
				if (stop.if_button_pressed()) {
					vector_space.draw_point(plot({ lamp1.position_.coords_.x, lamp1.position_.coords_.y, lamp1.position_.coords_.z }), 255 * lamp1.color_, R);
					vector_space.draw_point(plot({ lamp2.position_.coords_.x, lamp2.position_.coords_.y, lamp2.position_.coords_.z }), 255 * lamp2.color_, R);
					vector_space.draw_point(plot({ lamp3.position_.coords_.x, lamp3.position_.coords_.y, lamp3.position_.coords_.z }), 255 * lamp3.color_, R);
					vector_space.draw_point(plot({ lamp4.position_.coords_.x, lamp4.position_.coords_.y, lamp4.position_.coords_.z }), 255 * lamp3.color_, R);
					return 0;
				}
				Vector a({ x, y, 0 });
				if (sqrt(a.coords_.x * a.coords_.x + a.coords_.y * a.coords_.y) <= R) {
					a.coords_.z = sqrt(R * R - a.coords_.x * a.coords_.x - a.coords_.y * a.coords_.y);
					double degree = 50;
					Vector result = lamp1.lambert_speck(a, camera, color_of_ball, default_light, degree) +
									lamp2.lambert_speck(a, camera, color_of_ball, default_light, degree) +
									lamp3.lambert_speck(a, camera, color_of_ball, default_light, degree) +
									lamp4.lambert_speck(a, camera, color_of_ball, default_light, degree) +
									comp_mult(default_light, color_of_ball);
					result.custom_color();
					result = 255 * result;

					vector_space.draw_pixel({ a.coords_.x, a.coords_.y, a.coords_.z },
											{ result.coords_.x, result.coords_.y, result.coords_.z },
											(int)size_of_window);
											
				}
			}
		}
		vector_space.draw_point(plot({ lamp1.position_.coords_.x, lamp1.position_.coords_.y, lamp1.position_.coords_.z }), 255 * lamp1.color_, R);
		vector_space.draw_point(plot({ lamp2.position_.coords_.x, lamp2.position_.coords_.y, lamp2.position_.coords_.z }), 255 * lamp2.color_, R);
		vector_space.draw_point(plot({ lamp3.position_.coords_.x, lamp3.position_.coords_.y, lamp3.position_.coords_.z }), 255 * lamp3.color_, R);
		vector_space.draw_point(plot({ lamp4.position_.coords_.x, lamp4.position_.coords_.y, lamp4.position_.coords_.z }), 255 * lamp3.color_, R);
		txSleep();
	}
}


Vector Source::lambert_speck(Vector point_on_surface, Vector camera, Vector color_of_ball, Vector default_light, double degree) {
	Vector L1 = position_ + -1 * point_on_surface;
	double brightness = point_on_surface * L1 / (point_on_surface.length() * L1.length());
	if (brightness < 0) brightness = 0;

	Vector G = camera + -1 * point_on_surface;
	Vector L2 = L1.length() * 2 * brightness * (1 / point_on_surface.length()) * point_on_surface + (-1 * L1);
	double cos_G_L2 = G * L2 / (G.length() * L2.length());
	if (cos_G_L2 < 0 || position_.coords_.z <= -150) cos_G_L2 = 0;
	double speck = pow(cos_G_L2, degree);

	Vector lebeg_color = brightness * color_of_ball;
	Vector result = comp_mult(lebeg_color, color_) + speck * color_;
	result.custom_color();
	return result;
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
	return 1. / length * temp_vector;
}


void Vector::custom_color() {
	if ((*this).coords_.x > 1)
		(*this).coords_.x = 1;
	if ((*this).coords_.y > 1)
		(*this).coords_.y = 1;
	if ((*this).coords_.z > 1)
		(*this).coords_.z = 1;
}


plot CoordSys::to_pixels(plot coords) {
	plot start_of_coord = { coords1_.x / 2, coords1_.y / 2 };
	plot rec_coord = { coords.x * scaleX_ + start_of_coord.x, start_of_coord.y - coords.y * scaleY_ };
	return rec_coord;
}


void CoordSys::draw_point(plot coords, Vector color_of_point, double R) {
	txSetColor(TX_TRANSPARENT);
	txSetFillColor(RGB(color_of_point.coords_.x, color_of_point.coords_.y, color_of_point.coords_.z));
	int point_radius = (coords.z + 350) / 40;
	plot rec_coord = to_pixels(coords);
	if (sqrt(coords.x * coords.x + coords.y * coords.y) > R || coords.z > 0) {
		txCircle(rec_coord.x, rec_coord.y, point_radius);
	}
}


void CoordSys::draw_pixel(plot coords, plot color_of_point, double size_of_window) {
	plot rec_coord = to_pixels(coords);
	RGBQUAD* pixel = &Video_memory[(int)rec_coord.x + ((int)size_of_window - (int)rec_coord.y) * (int)size_of_window];
	pixel->rgbRed = color_of_point.x;
	pixel->rgbGreen = color_of_point.y;
	pixel->rgbBlue =color_of_point.z;
}


void CoordSys::draw_window() {
	txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
}


void CoordSys::set_color_back(Vector background) {
	txSetFillColor(RGB(background.coords_.x, background.coords_.y, background.coords_.z));
}


void Button::draw_button() {
	txSetColor(TX_LIGHTGREEN);
	txSetFillColor(TX_BLACK);
	txRectangle(coords0_.x, coords0_.y, coords1_.x, coords1_.y);
	txDrawText(coords0_.x, coords0_.y, coords1_.x, coords1_.y, name_);
}


bool Button::if_button_pressed() {
	if (txMouseButtons() == 1 &&
		txMouseX() >= coords0_.x &&
		txMouseX() <= coords1_.x &&
		txMouseY() >= coords0_.y &&
		txMouseY() <= coords1_.y) {
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
	return (a.coords_.x * b.coords_.x + a.coords_.y * b.coords_.y + a.coords_.z * b.coords_.z);
}


Vector comp_mult(Vector a, Vector b) {
	Vector result({ a.coords_.x * b.coords_.x, a.coords_.y * b.coords_.y, a.coords_.z * b.coords_.z });
	return result;
}

