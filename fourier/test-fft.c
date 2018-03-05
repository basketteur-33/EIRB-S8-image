/**
* @file test-fft.c
* @brief test the behaviors of functions in fft module
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <fft.h>

/**
* @brief test the forward and backward functions
* @param char* name, the input image file name
*/
void
test_forward_backward(char* name)
{
    fprintf(stderr, "test_forward_backward: ");
    pnm img = pnm_load(name);
    int rows = pnm_get_height(img);
    int cols = pnm_get_width(img);
    int size = rows * cols;
    unsigned short *data = pnm_get_image(img);
    fftw_complex *freq_repr = forward(rows, cols, data);
    unsigned short *img_back = backward(rows, cols, freq_repr);
    for (int i = 0; i < size; i++) {
        data[i] = img_back[i];
    }
    pnm_save(img, PnmRawPpm, "FB-res.ppm");
    fprintf(stderr, "OK\n");
}

/**
* @brief test image reconstruction from of magnitude and phase spectrum
* @param char *name: the input image file name
*/
void
test_reconstruction(char* name)
{
    fprintf(stderr, "test_reconstruction: ");

    pnm img = pnm_load(name);
    int rows = pnm_get_height(img);
    int cols = pnm_get_width(img);
    int size = rows * cols;
    unsigned short *data = pnm_get_image(img);
    fftw_complex *freq_repr = forward(rows, cols, data);

    float *as = malloc(sizeof(float) * size);
    float *ps = malloc(sizeof(float) * size);

    freq2spectra(rows, cols, freq_repr, as, ps);
    spectra2freq(rows, cols,as, ps, freq_repr);

    unsigned short *img_back = backward(rows, cols, freq_repr);
    for (int i = 0; i < size; i++) {
        data[i] = img_back[i];
    }
    pnm_save(img, PnmRawPpm, "FB-ASPS-res.ppm");
    fprintf(stderr, "OK\n");
}

/**
* @brief test construction of magnitude and phase images in ppm files
* @param char* name, the input image file name
*/
void
test_display(char* name)
{
    fprintf(stderr, "test_display: ");

    pnm img = pnm_load(name);
    int rows = pnm_get_height(img);
    int cols = pnm_get_width(img);
    int size = rows * cols;
    unsigned short *data = pnm_get_image(img);
    fftw_complex *freq_repr = forward(rows, cols, data);

    float *as = malloc(sizeof(float) * size);
    float *ps = malloc(sizeof(float) * size);

    freq2spectra(rows, cols, freq_repr, as, ps);

    pnm img_as = pnm_new(cols, rows, PnmRawPpm);
    pnm img_ps = pnm_new(cols, rows, PnmRawPpm);

    unsigned short *data_as = pnm_get_image(img_as);
    unsigned short *data_ps = pnm_get_image(img_ps);
    float as_max = .0;
    float ps_max = .0;
    for (int i = 0; i < size; i++) {
        if (as_max < as[i]) {
            as_max = as[i];
        }
        if (ps_max < ps[i]) {
            ps_max = ps[i];
        }
    }
    for (int i = 0; i < size; i++) {
        for (int k = 0; k < 3; k++) {
            data_as[i*3+k] = 255 * pow(as[i]/as_max, .1);
            data_ps[i*3+k] = 255 * pow(ps[i]/ps_max, .1);
            // data_as[i*3+k] = log10(1+as[i]);
            // data_ps[i*3+k] = log10(1+ps[i]);
        }
    }


    pnm_save(img_as, PnmRawPpm, "AS-res.ppm");
    pnm_save(img_ps, PnmRawPpm, "PS-res.ppm");
    fprintf(stderr, "OK\n");
}

/**
* @brief test the modification of magnitude by adding a periodic functions
on both vertical and horizontal axis, and
*        construct output images
* @param char* name, the input image file name
*/
void

test_add_frequencies(char* name)
{
    fprintf(stderr, "test_add_frequencies: ");
    (void)name;
    fprintf(stderr, "OK\n");
}

void
run(char* name)
{
    test_forward_backward(name);
    test_reconstruction(name);
    test_display(name);
    test_add_frequencies(name);
}

void
usage(const char *s)
{
    fprintf(stderr, "Usage: %s <ims> \n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 1
int
main(int argc, char *argv[])
{
    if (argc != PARAM+1) usage(argv[0]);
    run(argv[1]);
    return EXIT_SUCCESS;
}