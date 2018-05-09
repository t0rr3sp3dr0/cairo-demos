#include <math.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

static void do_drawing(cairo_t *);

struct {
    gboolean timer;
    int count;
    int cycle;
    int velocity;
    double coordx[100];
    double coordy[100];
} glob;

static cairo_t *
begin_paint (GdkDrawable *window)
{
    Display *dpy;
    Drawable xid;
    Visual *visual;
    GdkDrawable *drawable;
    gint x_offset, y_offset;
    gint width, height;
    cairo_surface_t *surface;
    cairo_t *cr;

    if (GDK_IS_WINDOW (window))
	gdk_window_get_internal_paint_info (window, &drawable,
					&x_offset, &y_offset);
    else
	drawable = window;

    dpy = gdk_x11_drawable_get_xdisplay (drawable);
    xid = gdk_x11_drawable_get_xid (drawable);
    gdk_drawable_get_size (drawable, &width, &height);
    visual = GDK_VISUAL_XVISUAL(gdk_drawable_get_visual (drawable));
    surface = cairo_xlib_surface_create (dpy, xid, visual,
					 width, height);
    cr = cairo_create (surface);
    cairo_surface_destroy (surface);

    if (GDK_IS_WINDOW (window))
	cairo_translate (cr, -x_offset, -y_offset);

    return cr;
}

static void
end_paint (cairo_t *cr)
{
    cairo_destroy (cr);
}


static gboolean
on_expose_event (GtkWidget	*widget,
		 GdkEventExpose *event,
		 gpointer	 data)
{
    cairo_t *cr;

    cr = begin_paint (widget->window);
    cairo_save (cr);
    {
	do_drawing (cr);
    }
    cairo_restore (cr);
    end_paint (cr);

    return TRUE;
}

static void draw_green_square(cairo_t *cr, int s)
{
    cairo_set_source_rgb(cr, 0, 0.5, 0);
    cairo_set_line_width(cr, 2);

    cairo_rel_line_to(cr, 0, s/2);
    cairo_rel_line_to(cr, s/2, s/2);
    cairo_rel_line_to(cr, s/2, -s/2);
    cairo_rel_line_to(cr, 0, -s/2);
    cairo_close_path(cr);

    cairo_stroke(cr);
}

/*
static void draw_green_square_abs(cairo_t *cr, int s, int x, int y)
{
    cairo_set_source_rgb(cr, 0, 0.5, 0);
    cairo_set_line_width(cr, 2);

    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x+0,   y+s/2);
    cairo_line_to(cr, x+s/2, y+s);
    cairo_line_to(cr, x+s,   y+s/2);
    cairo_line_to(cr, x+s,   y);
    cairo_close_path(cr);

    cairo_stroke(cr);
}
*/

static void draw_group_box(cairo_t *cr, int x, int y, const char *text, int height)
{
    // Group box
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_set_line_width (cr, 1);
    cairo_rectangle (cr, x-5, y-5, 385, height);
    cairo_stroke (cr);

    cairo_select_font_face(cr, "Courier",
			   CAIRO_FONT_SLANT_NORMAL,
			   CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 12);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_move_to(cr, x, y+10);
    cairo_text_path(cr, text);
    cairo_fill(cr);
}

static void draw_label(cairo_t *cr, int x, int y, const char *text)
{
    cairo_select_font_face(cr, "Courier",
			   CAIRO_FONT_SLANT_NORMAL,
			   CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 12);
    cairo_set_source_rgb(cr, 0, 0, 1);
    cairo_move_to(cr, x, y);
    cairo_text_path(cr, text);
    cairo_fill(cr);
}

static void do_drawing(cairo_t *cr)
{
    int x = 10;
    int y = 10;
    int s = 30;
    double param;
    cairo_matrix_t matrix;
    cairo_matrix3_t *matrix3 = cairo_matrix3_create();

    glob.cycle += glob.velocity;
    if (glob.cycle <= -100 || glob.cycle >= 100) {
	glob.velocity *= -1;
    }

    // Background
    cairo_paint (cr);

    /* ------------------------------------------------------------ */
    draw_group_box (cr, x, y, "BASIC TRANSFORMATIONS", 190);
    y += 30;

    // Untransformed
    cairo_move_to(cr, x, y);
    cairo_save (cr);
    draw_green_square (cr, s);
    cairo_restore (cr);
    draw_label (cr, x+3*s, y+s/2, "Untransformed");
    y += s + 10;

    // Scaled
    param = 1.0 + glob.cycle/300.0;
    cairo_move_to(cr, x,   y);
    cairo_save (cr);
    cairo_scale (cr, param, param);
    draw_green_square (cr, s);
    cairo_restore (cr);
    draw_label (cr, x+3*s, y+s/2, "Scaled");
    y += s + 10;

    // Rotated half a radian
    param = glob.cycle / 200.0;
    cairo_save (cr);
    cairo_move_to(cr, x, y);
    cairo_rotate (cr, param);
    draw_green_square (cr, s);
    cairo_restore (cr);
    draw_label (cr, x+3*s, y+s/2, "Rotated 1/2 Radian");
    y += s + 10;

    // Translated
    param = s * (0.5 + glob.cycle / 200.0);
    cairo_save (cr);
    cairo_translate (cr, param, 0);
    cairo_move_to(cr, x,   y);
    draw_green_square (cr, s);
    cairo_restore (cr);
    draw_label (cr, x+3*s, y+s/2, "Translated");
    y += s + 10;

    y += s + 10;

    /* ------------------------------------------------------------ */
    draw_group_box (cr, x, y, "2D MATRIX TRANSFORMATIONS", 360);
    y += 30;

    // Identity matrix - Untransformed
    cairo_matrix_init_identity (&matrix);
    cairo_transform (cr, &matrix);
    cairo_move_to(cr, x,   y);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Identity matrix");
    y += s + 10;

    // Scaling matrix
    param = 1.0 + glob.cycle/300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix_init (&matrix,
		       param, 0.0,
		       0.0, param,
		       0.0, 0.0);
    cairo_transform (cr, &matrix);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Scaling matrix");
    y += s + 10;

    // Rotation matrix
    param = glob.cycle / 200.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix_init (&matrix,
		       cos(param), -sin(param),
		       sin(param), cos(param),
		       0.0, 0.0);
    cairo_transform (cr, &matrix);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Rotational matrix");
    y += s + 10;

    // Translation matrix
    param = s * (1 + glob.cycle / 100.0);
    cairo_matrix_init_translate (&matrix, param, 0);
    cairo_set_matrix (cr, &matrix);
    cairo_move_to(cr, x,   y);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Translation matrix");
    y += s + 10;

    // Inversion matrix
    if (glob.cycle < 0)
	param = -1;
    else
	param = 1;
    cairo_move_to(cr, x,   y + 0.5 * (1 - param) * s );  // Correction for drawing upside down
    cairo_matrix_init (&matrix,
		       1.0, 0.0,
		       0.0, param,
		       0.0, 0.0);
    cairo_transform (cr, &matrix);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Inversion matrix");
    y += s + 10;

    // Y Shear matrix
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix_init (&matrix,
		       1.0, 0.0,
		       param, 1.0,
		       0.0, 0.0);
    cairo_transform (cr, &matrix);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Vertical shear matrix");
    y += s + 10;

    // X Shear matrix
    param = 0.5 + glob.cycle / 300.0;
    cairo_matrix_init (&matrix,
		       1.0, param,
		       0.0, 1.0,
		       0.0, 0.0);
    cairo_transform (cr, &matrix);
    cairo_move_to(cr, x,   y);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Horizontal shear matrix");
    y += s + 10;

    /* ------------------------------------------------------------ */
    x += 420;
    y = 10;
    draw_group_box (cr, x, y, "PROJECTIVE TRANSFORMATIONS", 500);
    y += 30;

    // Identity matrix - Untransformed
    cairo_matrix3_init_identity (matrix3);
    cairo_set_matrix3 (cr, matrix3); // TODO
    cairo_move_to(cr, x,   y);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Identity matrix");
    y += s + 10;

    // X projection
    param = 0.5 + glob.cycle / 200.0;
    cairo_move_to(cr, x + 10,   y);
    cairo_matrix3_init (matrix3,
			1.0, 0.0,
			0.0, 1.0,
			0.0, 0.0,
			0.001 * param, 0.0);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Horizontal identity projection");
    y += s + 10;

    // Y projection
    param = 0.5 + glob.cycle / 200.0;
    cairo_move_to(cr, x + 10,   y);
    cairo_matrix3_init (matrix3,
			1.0, 0.0,
                        0.0, 1.0,
                        0.0, 0.0,
                        0.0, 0.00125 * param);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Vertical identity projection");
    y += s + 10;

    // Scaling matrix with X projection
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			param, 0.0,
			0.0, param,
			0.0, 0.0,
			0.00001 * param, 0.0);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Scaling X projection");
    y += s + 10;

    // Scaling matrix with Y projection
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			param, 0.0,
			0.0, param,
			0.0, 0.0,
			0.0, -0.00001 * param);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Scaling Y projection");
    y += s + 10;

    // Rotation matrix with X projection
    param = 0.5 + glob.cycle / 200.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			cos(param), -sin(param),
			sin(param), cos(param),
			0.0, 0.0,
			0.0, -0.00001 * param);
    cairo_set_matrix3 (cr, matrix3);

    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Rotational matrix");
    y += s + 10;

    // Y Shear matrix w/ X projection
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			1.0, 0.0,
			param, 1.0,
			0.0, 0.0,
			0.001 * param, 0.0);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Vertical shear with X projection");
    y += s + 10;

    // Y Shear matrix w/ Y projection
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			1.0, 0.0,
			param, 1.0,
			0.0, 0.0,
			0.0, 0.001 * param);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Vertical shear with Y projection");
    y += s + 10;

    // X Shear matrix w/ X projection
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			1.0, param,
			0.0, 1.0,
			0.0, 0.0,
			0.00005 * param, 0.0);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Horizontal shear matrix with X projection");
    y += s + 10;

    // X Shear matrix w/ Y projection
    param = 0.5 + glob.cycle / 300.0;
    cairo_move_to(cr, x,   y);
    cairo_matrix3_init (matrix3,
			1.0, param,
			0.0, 1.0,
			0.0, 0.0,
			0.0, 0.0001 * param);
    cairo_set_matrix3 (cr, matrix3);
    draw_green_square (cr, s);
    cairo_identity_matrix (cr);
    draw_label (cr, x+3*s, y+s/2, "Horizontal shear matrix with Y projection");
    y += s + 10;

    cairo_matrix3_destroy (matrix3);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
			gpointer user_data)
{
    printf("Clicked %f %f on button %d\n", event->x, event->y, event->button);
    if (event->button == 1) {
	glob.coordx[glob.count] = event->x;
	glob.coordy[glob.count++] = event->y;
	glob.timer = !glob.timer;
    }

    if (event->button == 3) {
	gtk_widget_queue_draw(widget);
    }

    return TRUE;
}

static gboolean time_handler(GtkWidget *widget)
{
    if (!glob.timer) return FALSE;
    gtk_widget_queue_draw(widget);

    return TRUE;
}


int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *darea;

    glob.timer = TRUE;
    glob.count = 0;
    glob.cycle = -100;
    glob.velocity = 5;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(G_OBJECT(darea), "expose_event",
		     G_CALLBACK(on_expose_event), NULL);
    g_signal_connect(window, "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(window, "button-press-event",
		     G_CALLBACK(clicked), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 816, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Transformations");

    g_timeout_add(50, (GSourceFunc) time_handler, (gpointer) window);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
