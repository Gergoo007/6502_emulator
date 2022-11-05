#include "interface.h"

#include "cpu.h"
#include "memory.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <stdio.h>
 
#include <epoxy/gl.h>

#include <string.h>

//#define APPLICATION_ID "me.gergoo007.6502emu"

#include "../gresources/gresource.h"

#define FB_SIZE 64 * 8

uint8_t kill_emu_thread = 1;

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

	GtkGLArea* framebuffer;
} emulator_gui;

emulator_gui gui_struct;

void exit_app(GtkApplication* app, gpointer data) {
	gtk_window_destroy(GTK_WINDOW(data));
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

gboolean run_emu() {
	if(!kill_emu_thread)
		cpu_glob.exec_by_step(2, &mem_glob);
	
	return TRUE;
}

static void run_btn_clicked(GtkWidget* widget, gpointer data) {
	kill_emu_thread = 0;
	
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

static void step_btn_clicked(GtkWidget* widget, gpointer data) {
	cpu_glob.exec_by_step(1, &mem_glob);
}

static void reset_btn_clicked(GtkWidget* widget, gpointer data) {
	cpu_glob.reset(&mem_glob);
	update_display();
}

void update_display() {
	char text[255];

	sprintf(text, "A: 0x%02x ", cpu_glob.A);
	gtk_label_set_text(GTK_LABEL(gui_struct.A), text);

	sprintf(text, "X: 0x%02x ", cpu_glob.X);
	gtk_label_set_text(GTK_LABEL(gui_struct.X), text);

	sprintf(text, "Y: 0x%02x ", cpu_glob.Y);
	gtk_label_set_text(GTK_LABEL(gui_struct.Y), text);

	sprintf(text, "PC: 0x%04x ", cpu_glob.PC);
	gtk_label_set_text(GTK_LABEL(gui_struct.PC), text);

	sprintf(text, "P: 0b00100000");
	gtk_label_set_text(GTK_LABEL(gui_struct.P), text);

	gtk_gl_area_queue_render(gui_struct.framebuffer);

	GtkTextBuffer* rom_buff = gtk_text_buffer_new(NULL);
	parse_rom(&mem_glob);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(rom_buff), rom_buffer, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(gui_struct.rom), GTK_TEXT_BUFFER(rom_buff));

	GtkTextBuffer* ram_buff = gtk_text_buffer_new(NULL);
	parse_ram(&mem_glob);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ram_buff), ram_buffer, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(gui_struct.ram), GTK_TEXT_BUFFER(ram_buff));
}

// 256 colors: each pixel occupies 8 bits
// 1 byte will be 1 pixel
static gboolean render(GtkGLArea *area, GdkGLContext *context) {
	byte framebuffer[64][64];
	memcpy(framebuffer, &(mem_glob.data[0x2000]), 64*64);

	typedef struct {
		byte b : 2;
		byte g : 3;
		byte r : 3;
	} pixel; // 8 bits or 1 byte

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_SCISSOR_TEST);
	for (uint16_t x = 0; x < 64; x++) {		// row (x)
		for (uint16_t y = 0; y < 64; y++) {	// column (y)
			pixel _pixel;
			memcpy(&_pixel, &(framebuffer[y][x]), 1);
			
			glScissor(x * 8, (63 - y) * 8, 8, 8);

			glClearColor((float) _pixel.r / 8, (float) _pixel.g / 8, (float) _pixel.b / 4, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			// Remember to disable scissor test, or, perhaps reset the scissor rectangle:
		}
	}
	glDisable(GL_SCISSOR_TEST);

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
	gtk_widget_add_tick_callback(GTK_WIDGET(window), run_emu, NULL, NULL);

	GObject* framebuffer = gtk_builder_get_object(builder, "framebuffer");
	gtk_widget_set_size_request(GTK_WIDGET(framebuffer), 64 * 8, 64 * 8);
	g_signal_connect(framebuffer, "render", G_CALLBACK(render), NULL);

	// Fill the ROM and RAM windows
	GObject* rom = gtk_builder_get_object(builder, "rom");
	gtk_text_view_set_editable(GTK_TEXT_VIEW(rom), FALSE);

	GObject* ram = gtk_builder_get_object(builder, "ram");
	gtk_text_view_set_editable(GTK_TEXT_VIEW(ram), TRUE);
	
	GObject* code = gtk_builder_get_object(builder, "code");

	GObject* run_btn = gtk_builder_get_object(builder, "run_btn");
	g_signal_connect(run_btn, "clicked", G_CALLBACK(run_btn_clicked), NULL);

	GObject* step_btn = gtk_builder_get_object(builder, "step_btn");
	g_signal_connect(step_btn, "clicked", G_CALLBACK(step_btn_clicked), NULL);

	GObject* reset_btn = gtk_builder_get_object(builder, "reset_btn");
	g_signal_connect(reset_btn, "clicked", G_CALLBACK(reset_btn_clicked), NULL);

	GObject* stop_btn = gtk_builder_get_object(builder, "stop_btn");
	g_signal_connect(stop_btn, "clicked", G_CALLBACK(stop_btn_clicked), NULL);
	gtk_widget_set_sensitive(GTK_WIDGET(stop_btn), FALSE);

	// Create register labels
	GObject* a_reg_label 	= gtk_builder_get_object(builder, "a_reg_lab");
	GObject* x_reg_label 	= gtk_builder_get_object(builder, "x_reg_lab");
	GObject* y_reg_label 	= gtk_builder_get_object(builder, "y_reg_lab");
	GObject* pc_reg_label 	= gtk_builder_get_object(builder, "pc_reg_lab");
	GObject* p_reg_label 	= gtk_builder_get_object(builder, "p_reg_lab");

	// Fill the GUI struct
	gui_struct.btn_step 	= GTK_BUTTON(step_btn);
	gui_struct.btn_stop 	= GTK_BUTTON(stop_btn);
	gui_struct.btn_run 		= GTK_BUTTON(run_btn);
	gui_struct.A 			= GTK_LABEL(a_reg_label);
	gui_struct.X 			= GTK_LABEL(x_reg_label);
	gui_struct.Y 			= GTK_LABEL(y_reg_label);
	gui_struct.PC 			= GTK_LABEL(pc_reg_label);
	gui_struct.P 			= GTK_LABEL(p_reg_label);
	gui_struct.code 		= GTK_TEXT_VIEW(code);
	gui_struct.ram 			= GTK_TEXT_VIEW(ram);
	gui_struct.rom 			= GTK_TEXT_VIEW(rom);
	gui_struct.framebuffer 	= GTK_GL_AREA(framebuffer);

	update_display();

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