/***
 * writen by - Jiafeng Du
 *
 * image apis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"

/* Constructors and destructors */

/***
 * Allocates an Image structure and initializes the top level fields to appropriate values.
 * Allocates space for an image of the specified size, unless either rows or cols is 0.
 * Returns a pointer to the allocated Image structure.
 * Returns a NULL pointer if the operation fails.
 */
Image *image_create(int rows, int cols)
{
    int r;

    Image *image = (Image *)malloc(sizeof(Image));
    if (image == NULL) return NULL;
    image->rows = rows;
    image->cols = cols;
    if (rows == 0 || cols == 0)
    {
        image->data = NULL;
        image->alpha = image->depth = NULL;
    }
    else
    {
        image->alpha = (float **)malloc(sizeof(float *) * rows);
        if (image->alpha == NULL)
        {
            free(image);
            return NULL;
        }
        image->alpha[0] = (float *)malloc(rows * cols * sizeof(float));
        if (image->alpha[0] == NULL)
        {
            free(image->alpha);
            free(image);
            return NULL;
        }
        for (r = 0; r < rows; r++)
        {
            image->alpha[r] = &(image->alpha[0][r * cols]);
        }
        image->depth = (float **)malloc(sizeof(float *) * rows);
        if (image->depth == NULL)
        {
            free(image->alpha[0]);
            free(image->alpha);
            free(image);
            return NULL;
        }
        image->depth[0] = (float *)malloc(rows * cols * sizeof(float));
        if (image->depth[0] == NULL)
        {
            free(image->alpha[0]);
            free(image->alpha);
            free(image->depth);
            free(image);
            return NULL;
        }
        for (r = 0; r < rows; r++)
        {
            image->depth[r] = &(image->depth[0][r * cols]);
        }
        image->data = (FPixel **)malloc(rows * (sizeof(FPixel *)));
        if (image->data == NULL)
        {
            free(image->alpha[0]);
            free(image->depth[0]);
            free(image->alpha);
            free(image->depth);
            free(image);
            return NULL;
        }
        image->data[0] = (FPixel *)malloc(rows * cols * (sizeof(FPixel)));
        if (image->data[0] == NULL)
        {
            free(image->alpha[0]);
            free(image->depth[0]);
            free(image->data[0]);
            free(image->data);
            free(image->alpha);
            free(image->depth);
            free(image);
        }
        for (r = 0; r < rows; r++)
        {
            image->data[r] = &(image->data[0][r * cols]);
        }
    }
    for (r = 0; r < image->rows; r++)
    {
        for (int c = 0; c < image->cols; c++)
        {
            image->depth[r][c] = 0.0f;
        }
    }
    image->maxval.rgb[0] = image->maxval.rgb[1] = image->maxval.rgb[2] = 0.0;
    image->filename = NULL;
    return image;
}

/***
 * de-allocates image data and frees the Image structure.
 */
void image_free(Image *src)
{
    if (src == NULL) // if src is null
    {
        return;
    }
    if (src->data == NULL && src->alpha == NULL && src->depth == NULL) // if nothing exist
    {
        free(src);
        return;
    }
    else if (src->data == NULL && src->alpha == NULL) // if only depth exist
    {
        if (src->depth[0] != NULL)
        {
            free(src->depth[0]);
        }
        free(src->depth);
        free(src);
        return;
    }
    else if (src->data == NULL && src->depth == NULL) // if only alpha exist
    {
        if (src->alpha[0] != NULL)
        {
            free(src->alpha[0]);
            free(src->alpha);
            free(src);
            return;
        }
    }
    else if (src->alpha == NULL && src->depth == NULL) // if only data exist
    {
        if (src->data[0] != NULL)
        {
            free(src->data[0]);
        }
        free(src->data);
        free(src);
        return;
    }
    else if (src->alpha == NULL) // if data, depth exist
    {

        if (src->data[0] != NULL)
        {
            free(src->data[0]);
        }
        if (src->depth[0] != NULL)
        {
            free(src->depth[0]);
        }
        free(src->data);
        free(src->depth);
        free(src);
        return;
    }
    else if (src->depth == NULL) // if data, alpha exist
    {
        if (src->data[0] != NULL)
        {
            free(src->data[0]);
        }
        if (src->alpha[0] != NULL)
        {
            free(src->alpha[0]);
        }
        free(src->data);
        free(src->alpha);
        free(src);
        return;
    }
    else if (src->data == NULL) // if alpha, depth exist
    {
        if (src->depth[0] != NULL)
        {
            free(src->depth[0]);
        }
        if (src->alpha[0] != NULL)
        {
            free(src->alpha[0]);
        }
        free(src->depth);
        free(src->alpha);
        free(src);
        return;
    }
    // everthing exist
    if (src->data[0] != NULL)
    {
        free(src->data[0]);
    }
    if (src->alpha[0] != NULL)
    {
        free(src->alpha[0]);
    }
    if (src->depth[0] != NULL)
    {
        free(src->depth[0]);
    }
    free(src->data);
    free(src->alpha);
    free(src->depth);
    free(src);
}

/***
 * given an uninitialized Image structure, sets the rows and cols fields to zero and the data field to NULL.
 */
void image_init(Image *src)
{
    src->rows = 0;
    src->cols = 0;
    src->data = NULL;
    
}

/***
 * allocates space for the image data given rows and columns and initializes the image data to appropriate values, such as 0.0 for RGB and
 * 1.0 for alpha and depth. Returns 0 if the operation is successful. Returns a non-zero value if the operation
 * fails. This function should free exist memory if rows and cols are both non-zero.
 */
int image_alloc(Image *src, int rows, int cols)
{
    int r, c;
    if (src->rows != 0 && src->cols != 0)
    { // free exist memory if rows and cols are both non-zero
        if (src->data[0] != NULL)
            free(src->data[0]);
        if (src->alpha[0] != NULL)
            free(src->alpha[0]);
        if (src->depth[0] != NULL)
            free(src->depth[0]);
        free(src->data);
        free(src->alpha);
        free(src->depth);
    }
    src->alpha = (float **)malloc(sizeof(float *) * rows);
    if (src->alpha == NULL)
    {
        return -1;
    }
    src->alpha[0] = (float *)malloc(cols * rows * (sizeof(float)));
    if (src->alpha[0] == NULL)
    {
        free(src->alpha);
        src->alpha = NULL;
        return -1;
    }
    src->depth = (float **)malloc(sizeof(float *) * rows);
    if (src->depth == NULL)
    {
        free(src->alpha[0]);
        free(src->alpha);
        src->alpha = NULL;
        return -1;
    }
    src->depth[0] = (float *)malloc(cols * rows * (sizeof(float)));
    if (src->depth[0] == NULL)
    {
        free(src->depth);
        src->depth = NULL;
        free(src->alpha[0]);
        free(src->alpha);
        src->alpha = NULL;
        return -1;
    }
    src->data = (FPixel **)malloc(rows * (sizeof(FPixel *)));
    if (src->data == NULL)
    {
        free(src->depth[0]);
        free(src->depth);
        free(src->alpha[0]);
        free(src->alpha);
        src->depth = NULL;
        src->alpha = NULL;
        return -1;
    }
    src->data[0] = (FPixel *)malloc(cols * rows * (sizeof(FPixel)));
    if (src->data[0] == NULL)
    {
        free(src->data[0]);
        free(src->data);
        free(src->depth[0]);
        free(src->depth);
        free(src->data);
        src->data = NULL;
        src->depth = NULL;
        src->alpha = NULL;
        return -1;
    }
    for (r = 0; r < rows; r++)
    { // set initial rgb to black
        for (c = 0; c < cols; c++)
        {
            src->data[r][c].rgb[0] = 0.0;
            src->data[r][c].rgb[1] = 0.0;
            src->data[r][c].rgb[2] = 0.0;
            src->alpha[r][c] = 1.0;
            src->depth[r][c] = 1.0;
        }
    }
    src->rows = rows;
    src->cols = cols;
    return 0;
}

/***
 * de-allocates image data and resets the Image structure fields.
 * The function does not free the Image structure.
 */
void image_dealloc(Image *src)
{
    free(src->alpha[0]);
    free(src->depth[0]);
    free(src->data[0]);
    free(src->data);
    free(src->depth);
    free(src->alpha);
    src->rows = src->cols = 0;
    src->data = NULL;
    src->alpha = src->depth = NULL;
}

/* I/O functions */

/***
 * reads a PPM image from the given filename.
 * Initializes the alpha channel to NULL and the z channel to 1.0;
 * Returns a NULL pointer if the operation fails
 */
Image *image_read(char *filename)
{
    char tag[40];
    FILE *fp;
    int read, num[3], curchar;
    int cols, rows;

    if (filename != NULL && strlen(filename))
    {
        fp = fopen(filename, "r");
    }
    else
    {
        return NULL;
    }

    if (fp)
    {
        fscanf(fp, "%s\n", tag);
        if (strncmp(tag, "P6", 40) != 0)
        {
            fprintf(stderr, "not a ppm!\n");
            fclose(fp);
            return NULL;
        }
        read = 0;
        while (read < 3)
        {
            curchar = fgetc(fp);
            if ((char)curchar == '#')
            {
                while (fgetc(fp) != '\n')
                    /* skip comments */;
            }
            else
            {
                ungetc(curchar, fp);
                fscanf(fp, "%d", &(num[read]));
                read++;
            }
        }
        while (fgetc(fp) != '\n')
            /* skip new line after header */;
        cols = num[0];
        rows = num[1];
        if (cols > 0 && rows > 0)
        {
            Image *image = image_create(rows, cols);
            if (image)
            {
                for (int r = 0; r < rows; r++)
                {
                    for (int c = 0; c < cols; c++)
                    {
                        size_t readBytes = fread(image->data[r][c].rgb, sizeof(FPixel), 3, fp);
                        if (readBytes != (size_t)(3))
                        {
                            fprintf(stderr, "Error reading pixel data from %s\n", filename);
                            image_free(image);
                            fclose(fp);
                            return NULL;
                        }
                        image->alpha[r][c] = 1.0f;
                        image->depth[r][c] = 0.0f;
                    }
                }
            }
            else
            {
                fprintf(stderr, "Error: Invalid image dimensions (%d, %d)\n", cols, rows);
            }
            if (fp != stdin)
            {
                fclose(fp);
            }
        }
    }
    else
    {
        fprintf(stderr, "Error opening file: %s\n", filename);
    }
    return NULL;
}

/***
 * writes a PPM image to the given filename.
 * Returns 0 on success.
 * Optionally, you can look at the filename extension and write different file types.
 */
int image_write(Image *src, char *filename)
{
    FILE *fp;
    if (filename != NULL && strlen(filename))
        fp = fopen(filename, "w");
    else
        fp = stdout;

    if (fp)
    {
        fprintf(fp, "P6\n");
        fprintf(fp, "%d %d\n255\n", src->cols, src->rows); // 1.000 is set arbitrarily for now
        for (int r = 0; r < src->rows; r++)
        {
            for (int c = 0; c < src->cols; c++)
            {
                // fprintf(fp,"%f %f %f", src->data[r][c].rgb[0], src->data[r][c].rgb[1], src->data[r][c].rgb[2]);
                unsigned char data[3];
                data[0] = src->data[r][c].rgb[0] > 1.0 ? 255 : (unsigned char)(src->data[r][c].rgb[0] * 255.0 + 0.5);
                data[1] = src->data[r][c].rgb[1] > 1.0 ? 255 : (unsigned char)(src->data[r][c].rgb[1] * 255.0 + 0.5);
                data[2] = src->data[r][c].rgb[2] > 1.0 ? 255 : (unsigned char)(src->data[r][c].rgb[2] * 255.0 + 0.5);
                //fwrite(src->data[r][c].rgb, sizeof(float), 3, fp);
                fwrite(data, sizeof(unsigned char), 3, fp);
            }
        }
    }
    else
    {
        return -1;
    }
    fclose(fp);
    return 0;
}

/* Access */

/***
 * returns the FPixel at (r, c).
 */
FPixel image_getf(Image *src, int r, int c)
{
    return src->data[r][c];
}

/***
 * returns the value of band b at pixel (r, c).
 */
float image_getc(Image *src, int r, int c, int b)
{
    return src->data[r][c].rgb[b];
}

/***
 * returns the alpha value at pixel (r, c).
 */
float image_geta(Image *src, int r, int c)
{
    return src->alpha[r][c];
}

/***
 * returns the depth value at pixel (r, c).
 */
float image_getz(Image *src, int r, int c)
{
    return src->depth[r][c];
}

/***
 * sets the values of pixel (r,c) to the FPixel val.
 */
void image_setf(Image *src, int r, int c, FPixel val) {
    src->data[r][c] = val;
}

/***
 * sets the value of pixel (r, c) band b to val.
 */
void image_setc(Image *src, int r, int c, int b, float val)
{
    src->data[r][c].rgb[b] = val;
}

/***
 * sets the alpha value of pixel (r, c) to val.
 */
void image_seta(Image *src, int r, int c, float val)
{
    src->alpha[r][c] = val;
}

/***
 * sets the depth value of pixel (r, c) to val.
 */
void image_setz(Image *src, int r, int c, float val)
{
    src->depth[r][c] = val;
}

/* Utility */

/***
 * resets every pixel to a default value (e.g. Black, alpha value of NULL, z value of NULL).
 */
void image_reset(Image *src)
{
    int r, c;
    FPixel black;
    black.rgb[0] = black.rgb[1] = black.rgb[2] = 0.0f;
    for (r = 0; r < src->rows; r++)
    {
        for (c = 0; c < src->cols; c++)
        {
            src->data[r][c] = black;
            src->alpha[r][c] = 1.0f;
            src->depth[r][c] = 1.0f;
        }
    }
}

/***
 * sets every FPixel to the given value.
 */
void image_fill(Image *src, FPixel val)
{
    int r, c;
    for (r = 0; r < src->rows; r++)
    {
        for (c = 0; c < src->cols; c++)
        {
            src->data[r][c] = val;
        }
    }
}

/***
 * sets the (r, g, b) values of each pixel to the given color.
 */
void image_fillrgb(Image *src, float r, float g, float b)
{
    int row, col;
    FPixel given;
    given.rgb[0] = r;
    given.rgb[1] = g;
    given.rgb[2] = b;
    for (row = 0; row < src->rows; row++)
    {
        for (col = 0; col < src->cols; col++)
        {
            src->data[row][col] = given;
        }
    }
}

/***
 * sets the alpha value of each pixel to the given value.
 */
void image_filla(Image *src, float a)
{
    int r, c;
    for (r = 0; r < src->rows; r++)
    {
        for (c = 0; c > src->cols; c++)
        {
            src->alpha[r][c] = a;
        }
    }
}

/***
 * sets the depth value of each pixel to the given value.
 */
void image_fillz(Image *src, float z)
{
    int r, c;
    for (r = 0; r < src->rows; r++)
    {
        for (c = 0; c > src->cols; c++)
        {
            src->depth[r][c] = z;
        }
    }
}