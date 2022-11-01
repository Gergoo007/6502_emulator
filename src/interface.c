#include "interface.h"

#include "cpu.h"
#include "memory.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <stdio.h>

#include <string.h>

//#define APPLICATION_ID "me.gergoo007.6502emu"

#define FB_SIZE 64 * 8

char ram_buffer[(0x8000 * 55) + 1], rom_buffer[(0x8000 * 55) + 1];

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

	for (uint32_t i = 0; i < 0x8000; i++) {
		char line[255];
		
		sprintf(line, "%04x: ", i);

		for (uint8_t j = 0; j < 16; j++) {
			char tmp[5];
			sprintf(tmp, "%02x ", j);
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

	for (uint32_t i = 0; i < 0x8000; i++) {
		char line[255];
		
		sprintf(line, "%04x: ", i);

		for (uint8_t j = 0; j < 16; j++) {
			char tmp[5];
			sprintf(tmp, "%02x ", j);
			strcat(line, tmp);
		}

		strcat(line, "\n");

		strcat(rom_buffer, line);
	}
}

static void run_btn_clicked(GtkWidget* widget, gpointer data) {
	//cpu_glob.exec_continous(&mem_glob);
	g_print("Hello, world! %x\n", 0x00);
}

static void step_btn_clicked(GtkWidget* widget, gpointer cpu) {
	cpu_glob.exec_by_step(1, &mem_glob);
	g_print("step!\n");
}

static void activate(GApplication* app, gpointer cpu) {
	GtkBuilder* builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "/home/gergoo007/.local/share/6502/gui.ui", NULL);

	GtkCssProvider* css = gtk_css_provider_new();
	gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(css), 
		"/home/gergoo007/.local/share/6502/default.css");
	
	gtk_style_context_add_provider_for_display(gdk_display_get_default(), 
		GTK_STYLE_PROVIDER(css), 1);

	GObject* window = gtk_builder_get_object(builder, "window");
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));

	GObject* framebuffer = gtk_builder_get_object(builder, "framebuffer");
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(framebuffer), 
		drawFB, NULL, NULL);

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
	
	GObject* run_btn = gtk_builder_get_object(builder, "run_btn");
	g_signal_connect(run_btn, "clicked", G_CALLBACK(run_btn_clicked), cpu);
	GObject* step_btn = gtk_builder_get_object(builder, "step_btn");
	g_signal_connect(step_btn, "clicked", G_CALLBACK(step_btn_clicked), cpu);

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