#define DEFAULT_AREA ENTIRE_SCREEN
#define DEFAULT_FORMAT PNG
#define DEFAULT_DELAY 250

// area crop options
enum {
	ENTIRE_SCREEN = 0,
	ACTIVE_WINDOW
};

// image output format options
enum {
	JPEG = 0,
	PNG,
	BMP
};
