#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct shape shape;

struct shape {
	double (*area)(shape *);
	double (*circumference)(shape *);
	int a;
	int b;
	int c;
	int r;
};

double area(shape *s)
{
	return s->area(s);
}

double area_square(shape *s)
{
	return s->a * s->a;
}

double area_circle(shape *s)
{
	return pow(s->r, 2.0) * M_PI;
}

double area_triangle(shape *s)
{
	double t = (s->a+s->b+s->c)/2;
	return sqrt(t * (t-s->a) * (t-s->b) * (t-s->c));
}

double circumference_square(shape *s)
{
	return 4 * s->a;
}

double circumference_circle(shape *s)
{
	return M_PI * 2 * s->r;
}

double circumference_triangle(shape *s)
{
	return s->a + s->b + s->c;
}

void create_square(shape *s, int a)
{
	s->area = &area_square;
	s->circumference = &circumference_square;
	s->a = a;
	printf("created square with side length %d\n", a);
}

void create_circle(shape *s, int r)
{
	s->area = &area_circle;
	s->circumference = &circumference_circle;
	s->r = r;
	printf("created circle with radius %d\n",r);
}

void create_triangle(shape *s, int a, int b, int c)
{
	s->area = &area_triangle;
	s->circumference = &circumference_triangle;
	s->a = a;
	s->b = b;
	s->c = c;
	printf("created triangle with side lengths %d,%d,%d\n", a, b, c);
}

int main()
{
	struct shape *s = (struct shape *)malloc(sizeof(struct shape));
	int i = 0;

	printf("What do you want to create?\n1: Square\n2: Circle\n3: Triangle\n");
	scanf("%i", &i);

	switch (i)
	{
		case 1:
		{
			int a = 0;
			printf("Enter the side length:\n");
			scanf("%d", &a);
			create_square(s, a);
			break;
		}
		case 2:
		{
			int r = 0;
			printf("Enter the radius:\n");
			scanf("%d", &r);
			create_circle(s, r);
			break;
		}
		case 3:
		{
			int a = 0;
			int b = 0;
			int c = 0;
			printf("Enter the side a:\n");
			scanf("%d", &a);
			printf("Enter the side b:\n");
			scanf("%d", &b);
			printf("Enter the side c:\n");
			scanf("%d", &c);
			create_triangle(s, a, b, c);
			break;
		}
		default:
			break;
	}

	printf("\narea is %f\n",s->area(s));
	printf("circumference is %f\n\n",s->circumference(s));
}