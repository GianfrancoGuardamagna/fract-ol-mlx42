#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "MLX42/include/MLX42/MLX42.h"
#include "ft_atoi.c"

#define WIDTH 512
#define HEIGHT 512

typedef struct {
	double	real;
	double	imag;
}	Complex;

Complex square(Complex z) {
    Complex res;
    res.real = z.real * z.real - z.imag * z.imag;
    res.imag = 2 * z.real * z.imag;
    return res;
}

Complex add(Complex a, Complex b) {
    Complex res;
    res.real = a.real + b.real;
    res.imag = a.imag + b.imag;
    return res;
}

static int g_fractal_type = 0;
static int g_max_iter = 100;
static float zoom = 1.0;

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while (i < n && s1[i] == s2[i] && s1[i] && s2[i])
		i++;
	if (i == n)
		return (0);
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
int	ft_strlen(const char *s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

static mlx_image_t* image;

int32_t ft_pixel(int32_t r, int32_t g, int32_t b, int32_t a)
{
    return (r << 24 | g << 16 | b << 8 | a);
}

int julia_iterator(Complex z, Complex c, int maxIter) {
    int i;

    i = 0;
    while (i < maxIter) {
        z = add(square(z), c);
        if (z.real * z.real + z.imag * z.imag > 4.0)
            return i;
        i++;
    }
    return maxIter;
}

int burning_ship_iterator(Complex c, int maxIter) {
    Complex z = {0, 0};
    int iter = 0;
    
    while (iter < maxIter) {
        z.real = fabs(z.real);
        z.imag = fabs(z.imag);
        z = add(square(z), c);
        
        if (z.real * z.real + z.imag * z.imag > 4.0)
            return iter;
        iter++;
    }
    return maxIter;
}

int mandelbrot_iterator(Complex c, int maxIter) {
    Complex z;
    int i;

    z.real = 0.0;
    z.imag = 0.0;
    i = 0;
    while (i < maxIter) {
        z = add(square(z), c);
        if (z.real * z.real + z.imag * z.imag > 4.0)
            return i;
        i++;
    }
    return maxIter;
}

uint32_t get_color(int iterations, int maxIter) {
    if (iterations == maxIter)
    {
        return ft_pixel(20, 20, 20, 255);
    }
    else
    {
        int r = (iterations * 255) / maxIter;
        int g = (iterations * 128) / maxIter;
        int b = (iterations * 64) / maxIter;
        return ft_pixel(r, g, b, 255);
    }
}

void ft_fractal(void* param)
{
    (void)param;
    double range = 4.0 / zoom;
    int iter = 0;
    Complex julia_c = {-0.7, 0.27015}; // Default Julia set constant

	for (uint32_t x = 0; x < image->width; ++x)
	{
		for (uint32_t y = 0; y < image->height; ++y)
		{
            double real = (x - WIDTH / 2.0) * range / WIDTH;
            double imag = (y - HEIGHT / 2.0) * range / HEIGHT;
            Complex point = {real, imag};
            
            if (g_fractal_type == 0)
                iter = mandelbrot_iterator(point, g_max_iter);
            else if (g_fractal_type == 1)
                iter = julia_iterator(point, julia_c, g_max_iter);
            else if (g_fractal_type == 2)
                iter = burning_ship_iterator(point, g_max_iter);
            
            uint32_t color = get_color(iter, g_max_iter);
            mlx_put_pixel(image, x, y, color);
		}
	}
}

void ft_hook(void* param)
{
	mlx_t* mlx = param;

	if (mlx_is_key_down(mlx, MLX_KEY_ESCAPE))
		mlx_close_window(mlx);
	if (mlx_is_key_down(mlx, MLX_KEY_UP))
    {
		zoom += 0.1;
        ft_fractal(NULL);
    }
	if (mlx_is_key_down(mlx, MLX_KEY_DOWN))
    {
		zoom -= 0.1;
        ft_fractal(NULL);
    }
}

// -----------------------------------------------------------------------------

//Falta que reciba el C de Julia por parametros
//Falta que utilice las ruedas del mouse en vez de las flechas para el zoom
//Falta que el zoom lo haga en la ubicacion del mouse
//Falta que se mueva por el fractal con las flechas
//Falta que cambie de colores
int32_t main(int argc, char **argv)
{
	mlx_t* mlx;

    // Check arguments
    if (argc < 3)
    {
        printf("Usage: %s <fractal_type> <max_iterations>\n", argv[0]);
        printf("Fractal types: mandelbrot, julia, burning_ship\n");
        return (EXIT_FAILURE);
    }

    // Parse fractal type
    if (!ft_strncmp(argv[1], "mandelbrot", ft_strlen(argv[1])))
        g_fractal_type = 0;
    else if (!ft_strncmp(argv[1], "julia", ft_strlen(argv[1])))
        g_fractal_type = 1;
    else if (!ft_strncmp(argv[1], "burning_ship", ft_strlen(argv[1])))
        g_fractal_type = 2;
    else
    {
        printf("Invalid fractal type. Use: mandelbrot, julia, or burning_ship\n");
        return (EXIT_FAILURE);
    }

    // Parse max iterations
    g_max_iter = ft_atoi(argv[2]);
    if (g_max_iter <= 0)
    {
        printf("Max iterations must be a positive number\n");
        return (EXIT_FAILURE);
    }

	// Initialize MLX
	if (!(mlx = mlx_init(WIDTH, HEIGHT, "Fractol", false)))
	{
		puts(mlx_strerror(mlx_errno));
		return(EXIT_FAILURE);
	}
	if (!(image = mlx_new_image(mlx, WIDTH, HEIGHT)))
	{
		mlx_close_window(mlx);
		puts(mlx_strerror(mlx_errno));
		return(EXIT_FAILURE);
	}
	if (mlx_image_to_window(mlx, image, 0, 0) == -1)
	{
		mlx_close_window(mlx);
		puts(mlx_strerror(mlx_errno));
		return(EXIT_FAILURE);
	}

    // Generate the fractal once
    ft_fractal(NULL);

    // Set up hooks
    mlx_loop_hook(mlx, ft_hook, mlx);

    mlx_loop(mlx);
    mlx_terminate(mlx);
    
	return (EXIT_SUCCESS);
}
