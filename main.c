#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "minilibx/mlx.h"

#define WINX 1920
#define WINY 1080
#define WIDTH_DRAW 3.5

typedef struct  s_mlx
{
	void	*mlx_ptr;
	void	*mlx_win;
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}				t_mlx;

typedef struct	s_data
{
	t_mlx		*mlx;
	int			zoom_in;
	int			zoom_out;
	int			exit;
}				t_data;

int	key_pressed(int keycode, t_data *data)
{
	if (keycode == 126)
		data->zoom_in = 1;
	if (keycode == 125)
		data->zoom_out = 1;
	if (keycode == 53)
		data->exit = 1;
	return (0);
}

int	key_released(int keycode, t_data *data)
{
	if (keycode == 126)
		data->zoom_in = 0;
	if (keycode == 125)
		data->zoom_out = 0;
	return (0);
}

void	my_mlx_pixel_put(t_mlx *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

void	clean_window(t_mlx *mlx)
{
	int		y;
	int		x;
	int		color;

	color = 0x00000000;
	y = -1;
	x = -1;
	while (++y < WINY)
	{
		x = -1;
		while (++x < WINX)
			my_mlx_pixel_put(mlx, x, y, color);
	}
}

// void	draw_circle(t_data *data, int r, int center_x, int center_y, int color)
// {
// 	int		y;
// 	int		x;
// 	float	circle;
// 	float	r_sq;

// 	y = -1;
// 	x = -1;
// 	r_sq = r * r;
// 	while (++y < WINY)
// 	{
// 		x = -1;
// 		while (++x < WINX)
// 		{
// 			circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
// 			if (circle >= (r_sq) - 1.1 * sqrt(r_sq) && circle <= (r_sq) + 1.1 * sqrt(r_sq))
// 				my_mlx_pixel_put(data->mlx, x, y, color);
// 		}
// 	}
// }

void	draw_circle(t_data *data, int r, int center_x, int center_y, int color)
{
	int		x;
	double	y;
	int		draw_y;
	float	r_sq;
	float	circle;

	r_sq = r * r;
	x = center_x - r;
	if (x < 0)
		x = 0;
	while (x <= center_x + r && x < WINX)
	{
		y = center_y + sqrt((r + x - center_x) * (r - x + center_x));
		circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
		draw_y = floor(y);
		while (circle >= (r_sq) - WIDTH_DRAW * sqrt(r_sq) && circle <= (r_sq) + WIDTH_DRAW * sqrt(r_sq) && draw_y < WINY && draw_y > 0)
		{
			my_mlx_pixel_put(data->mlx, x, draw_y, color);
			draw_y++;
			circle = ((x - center_x) * (x - center_x)) + ((draw_y - center_y) * (draw_y - center_y));
		}
		circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
		draw_y = floor(y);
		while (circle >= (r_sq) - WIDTH_DRAW * sqrt(r_sq) && circle <= (r_sq) + WIDTH_DRAW * sqrt(r_sq) && draw_y < WINY && draw_y > 0)
		{
			my_mlx_pixel_put(data->mlx, x, draw_y, color);
			draw_y--;
			circle = ((x - center_x) * (x - center_x)) + ((draw_y - center_y) * (draw_y - center_y));
		}
		y = center_y - sqrt((r + x - center_x) * (r - x + center_x));
		circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
		draw_y = floor(y);
		while (circle >= (r_sq) - WIDTH_DRAW * sqrt(r_sq) && circle <= (r_sq) + WIDTH_DRAW * sqrt(r_sq) && draw_y < WINY && draw_y > 0)
		{
			my_mlx_pixel_put(data->mlx, x, draw_y, color);
			draw_y++;
			circle = ((x - center_x) * (x - center_x)) + ((draw_y - center_y) * (draw_y - center_y));
		}
		circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
		draw_y = floor(y);
		while (circle >= (r_sq) - WIDTH_DRAW * sqrt(r_sq) && circle <= (r_sq) + WIDTH_DRAW * sqrt(r_sq) && draw_y < WINY && draw_y > 0)
		{
			my_mlx_pixel_put(data->mlx, x, draw_y, color);
			draw_y--;
			circle = ((x - center_x) * (x - center_x)) + ((draw_y - center_y) * (draw_y - center_y));
		}
		x++;
	}
}

void	draw_circle_rosace(t_data *data, int r, int center_x, int center_y, int rosace_r, int color)
{
	int		y;
	int		x;
	float	circle;
	float	r_sq;

	y = -1;
	x = -1;
	r_sq = rosace_r * rosace_r;
	while (++y < WINY)
	{
		x = -1;
		while (++x < WINX)
		{
			circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
			if (circle == r_sq)
				draw_circle(data, r, x, y, color);
		}
	}
}

void	draw_frequence(t_data *data, int frequence, uint64_t t, float offset, int rosace_r)
{
	int		r;
	int		color;

	r = frequence + 10 * sin(t * offset);
	color = 0xFF000000 * 1/(t/15 + frequence);
	// color = 0x000000FF << ((rand() % 4) * 8);
	draw_circle_rosace(data, r, WINX/2, WINY/2, rosace_r, color);
}

int	ft_close(t_data *data)
{
	free(data);
	exit(0);
}

void	draw_trippy_rosaces(t_data *data, uint64_t t)
{
	draw_frequence(data, 100, t, 0.1, 75);
	draw_frequence(data, 80, t, 0.07, 75);
	draw_frequence(data, 50, t, 0.05, 75);
	draw_frequence(data, 50, t, 0.02, 150);
	draw_frequence(data, 80, t, 0.032, 150);
	draw_frequence(data, 100, t, 0.023, 150);
}

void	draw_drops(t_data *data, uint64_t t, int x, int y, int frequence)
{
	int color;
	color = 0xFF000000 * 1/(t/15 + frequence);
	// color = 0x000000FF << ((rand() % 4) * 8);
	draw_circle(data, 1 * 0.5 * t, x, y,  color | (0x01000000 * t));	
}

void	draw_ondulation(t_data *data, uint64_t t, int x, int y)
{
	draw_drops(data, t % 250, x, y, 100);
	(t > 25 ? draw_drops(data, (t - 25) % 250, x, y, 100) : 1);
	(t > 50 ? draw_drops(data, (t - 50) % 250, x, y, 80) : 1);
	(t > 75 ? draw_drops(data, (t - 75) % 250, x, y, 50) : 1);
	(t > 100 ? draw_drops(data, (t - 100) % 250, x, y, 50) : 1);
	(t > 125 ? draw_drops(data, (t - 125) % 250, x, y, 50) : 1);
	(t > 150 ? draw_drops(data, (t - 150) % 250, x, y, 50) : 1);
	(t > 175 ? draw_drops(data, (t - 175) % 250, x, y, 80) : 1);
	(t > 200 ? draw_drops(data, (t - 200) % 250, x, y, 100) : 1);
	(t > 225 ? draw_drops(data, (t - 225) % 250, x, y, 100) : 1);
}

void	draw_ondulation_rosace(t_data *data, int center_x, int center_y, int rosace_r, uint64_t t)
{
	int		y;
	int		x;
	float	circle;
	float	r_sq;

	y = -1;
	x = -1;
	r_sq = rosace_r * rosace_r;
	while (++y < WINY)
	{
		x = -1;
		while (++x < WINX)
		{
			circle = ((x - center_x) * (x - center_x)) + ((y - center_y) * (y - center_y));
			if (circle == r_sq)
				draw_ondulation(data, t, x, y);
		}
	}
}

int screen(t_data *data)
{
	static int		r = 0;
	static uint64_t	t = 0;
	int				r_cpt;
	int				color;
	// int				frequencies1[28] = {0, 1, 2, 3, 2, 5, 10, 15, 7, 8, 1, 2, 3, 2, 5, 10, 15, 7, 8, 1, 2, 3, 2, 5, 10, 15, 7, 8};
	// int				frequencies2[28] = {0, 1, 2, 3, 2, 5, 10, 15, 7, 8, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2};
	// int				frequencies3[28] = {2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 2, 3, 2, 15, 7, 8, 1, 2, 3, 2, 5, 10, 15, 7, 8};

	if (data->exit)
		ft_close(data);
	if (data->zoom_in)
		r += 5;
	if (data->zoom_out)
		r -= 5;
    clean_window(data->mlx);
	r_cpt = r;
	// color = 69420;
	// while (r_cpt > 0)
	// {
	// 	color &= 0x00FFFFFF;
	// 	draw_circle_rosace(data, r_cpt, WINX/2, WINY/2, 50, color);
	// 	color *= 1.69;
	// 	if (color >= 0x00FFFFFF)
	// 		color = 69420;
	// 	r_cpt -= 100;
	// }
	// draw_circle_rosace(data, 800, WINX/2, WINY/2, 50, 0x00FF0000);
	// draw_circle_rosace(data, 600, WINX/2, WINY/2, 50, 0x0000FF00);
	// draw_circle_rosace(data, 400, WINX/2, WINY/2, 50, 0x000000FF);
	// draw_frequence(data, 850, t, 0.11);
	// draw_frequence(data, 800, t, 0.17);
	// draw_frequence(data, 750, t, 0.15);


	draw_ondulation_rosace(data, WINX/2, WINY/2, 150, t);
	// draw_trippy_rosaces(data, t);
	draw_ondulation(data, t, WINX/2, WINY/2);
	// draw_ondulation(data, t, 2*WINX/3, 2*WINY/3);
	// draw_ondulation(data, t, WINX/3, 2*WINY/3);
	// draw_ondulation(data, t, 2*WINX/3, WINY/3);

    mlx_put_image_to_window(data->mlx->mlx_ptr, data->mlx->mlx_win, data->mlx->img, 0, 0);
	mlx_do_sync(data->mlx->mlx_ptr);
	// usleep(100000);
	t++;
    return (0);
}

int main(void)
{
	t_data	*data;
    t_mlx   mlx;

	srand(time(NULL));
	data = malloc(sizeof(t_data));
	mlx.mlx_ptr = mlx_init();
	mlx.mlx_win = mlx_new_window(mlx.mlx_ptr, WINX, WINY, "Sound_Visual");
	mlx.img = mlx_new_image(mlx.mlx_ptr, WINX, WINY);
	mlx.addr = mlx_get_data_addr(mlx.img, &mlx.bits_per_pixel, &mlx.line_length, &mlx.endian);
	data->mlx = &mlx;
    data->zoom_in = 0;
    data->zoom_out = 0;
	data->exit = 0;
    mlx_hook(mlx.mlx_win, 02, 1L << 0, &key_pressed, data);
	mlx_hook(mlx.mlx_win, 03, 1L << 1, &key_released, data);
	mlx_hook(mlx.mlx_win, 17, 0, &ft_close, data);
	mlx_loop_hook(mlx.mlx_ptr, screen, data);
	mlx_loop(mlx.mlx_ptr);
	return (0);
}