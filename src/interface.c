#include "interface.h"

#include "cpu.h"
#include "memory.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <stdio.h>
 
#include <epoxy/gl.h>

#include <string.h>

//#define APPLICATION_ID "me.gergoo007.6502emu"

#include "../gresources/gresource.h"

#define FB_SIZE 64 * 8

uint8_t kill_emu_thread = 0;

char ram_buffer[(0x8000 * 55) + 1], rom_buffer[(0x8000 * 55) + 1];

typedef struct emulator_gui {
	GtkLabel* A;
	GtkLabel* X;
	GtkLabel* Y;
	GtkLabel* PC;
	GtkLabel* P;
	GtkLabel* cycles;

	GtkButton* btn_run;
	GtkButton* btn_step;
	GtkButton* btn_stop;

	GtkTextView* code;
	GtkTextView* ram;
	GtkTextView* rom;
} emulator_gui;

emulator_gui gui_struct;

void exit_app(GtkApplication* app, gpointer data) {
	gtk_window_destroy(GTK_WINDOW(data));
}

static void drawFB(GtkDrawingArea* fb, cairo_t* cr, int width, int height, gpointer data) {
	GdkRGBA color;
	gdk_rgba_parse(&color, "#000000");
	GtkStyleContext* context;

	context = gtk_widget_get_style_context(GTK_WIDGET(fb));

	//cairo_rectangle(cr, 0, 0, width, height);
	cairo_arc(cr, (double) width / 2, (double) height / 2, 60, 0, 4.5);

	cairo_rectangle(cr, 0, 0, width, height);

	printf("\n%d x %d\n", width, height);

	//gtk_style_context_get_color (context, &color);
	gdk_cairo_set_source_rgba (cr, &color);

	cairo_fill (cr);
}

//	RAM inside mem[] stretches from 0x0000 to 0x7fff;
//	0x8000 and everything above belongs to ROM
void parse_ram(Memory* mem) {
	// 	Number of characters in RAM window including \0 and \n
	//	0x8000: number of lines
	//	54: number of characters in one line including newlines
	//	+ 1 at the end to account for \0
	for (uint32_t i = 0; i < sizeof(ram_buffer); i++) {
		ram_buffer[i] = 0;
	}

	for (uint32_t i = 0; i < 0x8000; i += 16) {
		char line[255];
		
		sprintf(line, "%04x: ", i);

		for (uint8_t j = 0; j < 16; j++) {
			char tmp[5];
			sprintf(tmp, "%02x ", mem_glob.data[i + j]);
			strcat(line, tmp);
		}

		strcat(line, "\n");

		strcat(ram_buffer, line);
	}
}

void parse_rom(Memory* mem) {
	// 	Number of characters in RAM window including \0 and \n
	//	0x8000: number of lines
	//	54: number of characters in one line including newlines
	//	+ 1 at the end to account for \0
	for (uint32_t i = 0; i < sizeof(rom_buffer); i++) {
		rom_buffer[i] = 0;
	}

	for (uint32_t i = 0x8000; i < 0x10000; i += 16) {
		char line[255];
		
		sprintf(line, "%04x: ", i);

		for (uint8_t j = 0; j < 16; j++) {
			char tmp[5];
			sprintf(tmp, "%02x ", mem_glob.data[i + j]);
			strcat(line, tmp);
		}

		strcat(line, "\n");

		strcat(rom_buffer, line);
	}
}

void* launch(void* data) {
	cpu_glob.exec_continous(&mem_glob);

	pthread_exit(NULL);
}

pthread_t id;

static void run_btn_clicked(GtkWidget* widget, gpointer data) {
	kill_emu_thread = 0;

	pthread_create(&id, NULL, launch, NULL);
	//pthread_join(id, NULL);
	
	gtk_widget_set_sensitive(GTK_WIDGET(gui_struct.btn_run), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui_struct.btn_step), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui_struct.btn_stop), TRUE);
}

static void stop_btn_clicked(GtkWidget* widget, gpointer data) {
	gtk_widget_set_sensitive(GTK_WIDGET(gui_struct.btn_run), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui_struct.btn_step), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(gui_struct.btn_stop), FALSE);

	kill_emu_thread = 1;
}

static void step_btn_clicked(GtkWidget* widget, gpointer cpu) {
	cpu_glob.exec_by_step(1, &mem_glob);
	g_print("step!\n");
}

void on_step() {
	char text[255];

	sprintf(text, "A: 0x%02x ", cpu_glob.A);
	gtk_label_set_text(GTK_LABEL(gui_struct.A), text);

	sprintf(text, "X: 0x%02x ", cpu_glob.X);
	gtk_label_set_text(GTK_LABEL(gui_struct.X), text);

	sprintf(text, "Y: 0x%02x ", cpu_glob.Y);
	gtk_label_set_text(GTK_LABEL(gui_struct.Y), text);

	sprintf(text, "PC: 0x%04x ", cpu_glob.PC);
	gtk_label_set_text(GTK_LABEL(gui_struct.PC), text);

	//sprintf(text, "P: 0b%x", cpu_glob.P.B);
	sprintf(text, "P: 0b00100000");
	gtk_label_set_text(GTK_LABEL(gui_struct.P), text);
}

uint8_t framebuffer[64][64];

// 16 colors: each pixel occupies 4 bits
// 1 byte will be 2 pixels
static gboolean render(GtkGLArea *area, GdkGLContext *context) {
	framebuffer[10][10] = 10;
	framebuffer[10][11] = 10;
	framebuffer[10][12] = 10;

	/*glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor(100, 200, 8, 8);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	// Remember to disable scissor test, or, perhaps reset the scissor rectangle:
	glDisable(GL_SCISSOR_TEST); */

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	for (uint16_t i = 0; i < 64 * 64; i++) {
		
	}
	
	return TRUE;
}

static void activate(GApplication* app, gpointer cpu) {
	GResource* resources = gresource_get_resource();

	GtkBuilder* builder = gtk_builder_new_from_resource("/me/gergoo007/emu/main.ui");
	//gtk_builder_add_from_file(builder, "/home/gergoo007/.local/share/6502/gui.ui", NULL);

	GtkCssProvider* css = gtk_css_provider_new();
	gtk_css_provider_load_from_resource(GTK_CSS_PROVIDER(css), 
		"/me/gergoo007/emu/main.css");
	
	gtk_style_context_add_provider_for_display(gdk_display_get_default(), 
		GTK_STYLE_PROVIDER(css), 1);

	GObject* window = gtk_builder_get_object(builder, "window");
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	GObject* framebuffer = gtk_builder_get_object(builder, "framebuffer");
	gtk_widget_set_size_request(GTK_WIDGET(framebuffer), 64 * 8, 64 * 8);
	g_signal_connect(framebuffer, "render", G_CALLBACK(render), NULL);

	// Fill the ROM and RAM windows

	GObject* rom = gtk_builder_get_object(builder, "rom");
	GtkTextBuffer* rom_buff = gtk_text_buffer_new(NULL);
	parse_rom(&mem_glob);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(rom_buff), rom_buffer, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(rom), GTK_TEXT_BUFFER(rom_buff));

	GObject* ram = gtk_builder_get_object(builder, "ram");
	GtkTextBuffer* ram_buff = gtk_text_buffer_new(NULL);
	parse_ram(&mem_glob);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ram_buff), ram_buffer, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(ram), GTK_TEXT_BUFFER(ram_buff));
	
	GObject* code = gtk_builder_get_object(builder, "code");

	GObject* run_btn = gtk_builder_get_object(builder, "run_btn");
	g_signal_connect(run_btn, "clicked", G_CALLBACK(run_btn_clicked), cpu);

	GObject* step_btn = gtk_builder_get_object(builder, "step_btn");
	g_signal_connect(step_btn, "clicked", G_CALLBACK(step_btn_clicked), cpu);

	GObject* stop_btn = gtk_builder_get_object(builder, "stop_btn");
	g_signal_connect(stop_btn, "clicked", G_CALLBACK(stop_btn_clicked), cpu);
	gtk_widget_set_sensitive(GTK_WIDGET(stop_btn), FALSE);

	// Create register labels
	GObject* a_reg_label 	= gtk_builder_get_object(builder, "a_reg_lab");
	GObject* x_reg_label 	= gtk_builder_get_object(builder, "x_reg_lab");
	GObject* y_reg_label 	= gtk_builder_get_object(builder, "y_reg_lab");
	GObject* pc_reg_label 	= gtk_builder_get_object(builder, "pc_reg_lab");
	GObject* p_reg_label 	= gtk_builder_get_object(builder, "p_reg_lab");
	
	// Fill the GUI struct
	gui_struct.btn_step = GTK_BUTTON(step_btn);
	gui_struct.btn_stop = GTK_BUTTON(stop_btn);
	gui_struct.btn_run 	= GTK_BUTTON(run_btn);
	gui_struct.A 		= GTK_LABEL(a_reg_label);
	gui_struct.X 		= GTK_LABEL(x_reg_label);
	gui_struct.Y 		= GTK_LABEL(y_reg_label);
	gui_struct.PC 		= GTK_LABEL(pc_reg_label);
	gui_struct.P 		= GTK_LABEL(p_reg_label);
	gui_struct.code 	= GTK_TEXT_VIEW(code);
	gui_struct.ram 		= GTK_TEXT_VIEW(ram);
	gui_struct.rom 		= GTK_TEXT_VIEW(rom);

	gtk_widget_show(GTK_WIDGET(window));

	g_object_unref(builder);
}

int open_gui(int argc, char* argv[], CPU* cpu, Memory* mem) {
	GtkApplication* app;
	uint8_t status;

	app = gtk_application_new("me.gergoo007.emu", 0);
	g_signal_connect(app, "activate", G_CALLBACK(activate), cpu);

	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}