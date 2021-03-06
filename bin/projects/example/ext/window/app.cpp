#include "app.h"

#include "constants/data_path.h"

rbApp::rbApp(Rice::Object self)
: ofBaseApp()
{
	cout << "c++: constructor - window\n";
	
	// ofSetupOpenGL(1024,768,OF_WINDOW); // <-------- setup the GL context
	mSelf = self;
}

rbApp::~rbApp(){
	
}

void rbApp::setup(){
	ofSetDataPathRoot(DATA_PATH);
	
	
	
	// NOTE: Using full path to font on Ubuntu linux 16.04 works, but using just the name of the font does not work.
	// NOTE: DejaVu Sans just happens to be the default on my system, by whatever mechanism OpenFrameworks decides to use
	// (it's some some sort of FreeDesktop foundation standard API)
	// However, the first call here actually is not actually working right.
	ofTtfSettings settings("DejaVu Sans", 30);
	settings.antialiased = true;
	mFont.load(settings);
	
	ofTtfSettings s1("/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 20);
	s1.addRanges(ofAlphabet::Latin);
	s1.addRanges(ofAlphabet::Japanese);
	s1.antialiased = true;
	mFontU.load(s1);
	
	// NOTE: Liberation serif does not seem to have a "Regular" variant, at least not according to fc-list.
	ofTtfSettings s2("/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 20);
	s2.antialiased = true;
	mFont1.load(s2);
	
	ofTtfSettings s3("Liberation Serif", 20);
	s3.antialiased = true;
	mFont2.load(s3);
	
	
	// ofTtfSettings settings_mUnicodeFont("TakaoPGothic", 20);
	// settings_mUnicodeFont.antialiased = true;
	// settings_mUnicodeFont.addRanges({
	//     ofUnicode::Space,
	//     ofUnicode::Latin1Supplement,
	//     ofUnicode::LatinExtendedAdditional,
	//     ofUnicode::Hiragana,
	//     ofUnicode::Katakana,
	//     ofUnicode::KatakanaPhoneticExtensions,
	//     ofUnicode::CJKLettersAndMonths,
	//     ofUnicode::CJKUnified,
	// });
	// mUnicodeFont.load(settings_mUnicodeFont);
	
	
	
	
	mImage.load("box.jpg");
	// Short path works, even with dynamic library,
	// assuming you call ofSetDataPathRoot()
	// Otherwise, relative path fails for dynamic library
	// build of OpenFrameworks app, because relative paths
	// are relative to the main binary.
	// In standard oF, that's the app's executable.
	// In RubyOF, that's the ruby interpreter itself.
	
	
	
	// =====
	
	
	// need to call these before gui.setup()
	// ofxGuiSetFont("DejaVu Sans", 20, true, true);
	// ofxGuiSetFont("DejaVu Sans", 200, _bAntiAliased, _bFullCharacterSet, int dpi=0)
	
	ofxGuiSetTextPadding(4);
	ofxGuiSetDefaultWidth(400);
	ofxGuiSetDefaultHeight(40);

	
	
	// transforms.setName("visual transforms");
	// transforms.add(gui_scale.set("GUI scale", 1, 1, 16));
	// transforms.add(s.set("scale", 1, 1, 16));
	// transforms.add(x_pos.set("x", 0, 0, 800));
	// transforms.add(y_pos.set("y", 0, 0, 800));
	
	// gui_sections.add(transforms);
	
	// gui.setup(gui_sections);
	
	
	
	// TODO: should only call ruby-level setup function if C++ level setup finishes successfully. If there is some sort of error at this stage, any ruby-level actions will result in a segfault.
	mSelf.call("setup");
}

void rbApp::update(){
	mSelf.call("update");
}

void rbApp::draw(){
	ofPushMatrix();
	// ofLoadIdentityMatrix();
	
		ofPushStyle();
		
			ofColor color = ofColor::fromHex(0xFF0000, 0xFF);
			ofSetColor(color);
			
			
			int height;
			int x;
			int y;
			ofDrawBitmapString("Font Test", 0, 200, 0);
			
			x = 0;
			y = 300;
			height = mFont.getSize();
			mFont.drawString("mFontU", x, y);
			mFontU.drawString("Hand-woven pagent! こんにちは", x, y+height);
			
			x = 500;
			y = 300;
			height = mFont.getSize();
			mFont.drawString("mFont1", x, y);
			mFont1.drawString("Hand-woven pagent!", x, y+height);
			
			x = 500+300;
			y = 300;
			height = mFont.getSize();
			mFont.drawString("mFont2", x, y);
			mFont2.drawString("Hand-woven pagent!", x, y+height);
			
			
			
			
			// // // Draw some shapes
			// ofDrawRectangle(50, 50, 100, 100); // Top left corner at (50, 50), 100 wide x 100 high
			// ofDrawCircle(250, 100, 50); // Centered at (250, 100), radius of 50
			// ofDrawEllipse(400, 100, 80, 100); // Centered at (400 100), 80 wide x 100 high
			// ofDrawTriangle(500, 150, 550, 50, 600, 150); // Three corners: (500, 150), (550, 50), (600, 150)
			// ofDrawLine(700, 50, 700, 150); // Line from (700, 50) to (700, 150)
			
			
			
			
			// float height;
			
			// height = 11;
			// 	// ^ src: https://forum.openframeworks.cc/t/how-to-get-size-of-ofdrawbitmapstring/22578
			// ofDrawBitmapString("hello from C++!", 0, 0, 0);
		ofPopStyle();
		
		ofPushStyle();
			
			mImage.draw(600,400);
		
		ofPopStyle();
		
	ofPopMatrix();
	
	
	
	// gui.draw();
	
	
	
	mSelf.call("draw");
}


void rbApp::exit(){
	// ofApp::exit(); // no parent behavior for exit callback defined
	cout << "c++: exit\n";
	
	
	// ========================================
	// ========== add new stuff here ==========
	
	
	
	
	
	// ========================================
	// ========================================
	
	
	mSelf.call("on_exit");
}


void rbApp::keyPressed(int key){
	// Something seems to be consuming most keyboard events
	// when the application is started via the Ruby layer in Rake.
	// 
	// That problem prevents this funciton from being called,
	// and also prevents the app from closing when ESC is pressed,
	// like normal ofApp windows do
	// (including the window you get when you execute just the C++ layer of this very project)
	
	ofBaseApp::keyPressed(key);
	
	
	ofLog() << key;
	
	
	
	// ========================================
	// ========== add new stuff here ==========
	
	
	
	
	
	// ========================================
	// ========================================
	
	
	
	// TODO: consider listening for key symbols (the physical key buttons) as well / instead of this. Need to set up another hook into the oF event system to do that, but might be better / easier for setting up structural keybindings.
	mSelf.call("key_pressed", key);
}

void rbApp::keyReleased(int key){
	ofBaseApp::keyReleased(key);
	
	mSelf.call("key_released", key);
}

void rbApp::mouseMoved(int x, int y ){
	ofBaseApp::mouseMoved(x,y);
	
	mSelf.call("mouse_moved", x,y);
}

void rbApp::mouseDragged(int x, int y, int button){
	ofBaseApp::mouseDragged(x,y,button);
	
	mSelf.call("mouse_dragged", x,y, button);
}

void rbApp::mousePressed(int x, int y, int button){
	ofBaseApp::mousePressed(x,y,button);
	
	mSelf.call("mouse_pressed", x,y, button);
}

void rbApp::mouseReleased(int x, int y, int button){
	ofBaseApp::mouseReleased(x,y,button);
	
	mSelf.call("mouse_released", x,y, button);
}

void rbApp::mouseEntered(int x, int y){
	ofBaseApp::mouseEntered(x,y);
	
	mSelf.call("mouse_entered", x,y);
}

void rbApp::mouseExited(int x, int y){
	ofBaseApp::mouseExited(x,y);
	
	mSelf.call("mouse_exited", x,y);
}

void rbApp::mouseScrolled(int x, int y, float scrollX, float scrollY ){
	ofBaseApp::mouseScrolled(x,y, scrollX, scrollY);
	
	mSelf.call("mouse_scrolled", x,y, scrollX, scrollY);
}

void rbApp::windowResized(int w, int h){
	ofBaseApp::windowResized(w,h);
	
	mSelf.call("window_resized", w,h);
}

void rbApp::dragEvent(ofDragInfo dragInfo){
	// NOTE: drag event example works with Nautilus, but not Thunar (GLFW window)
	
	// https://github.com/openframeworks/openFrameworks/issues/1862
	// ^ this issue explains that Glut windows can not process file drag events on Linux
	
	ofBaseApp::dragEvent(dragInfo);
	
	
	// NOTE: dragInfo.files is a std::vector, not an array. Apparently, Rice doesn't understand how to convert that into a Ruby array? so I guess that needs to be done manually...
	
	// ./test.rb:190:in `show': Unable to convert std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >* (ArgumentError)
	
	Rice::Array filepaths;
	
	for(std::__cxx11::basic_string<char>& e : dragInfo.files){
		filepaths.push(to_ruby(e));
	}

	mSelf.call("drag_event", filepaths, dragInfo.position);
}

void rbApp::gotMessage(ofMessage msg){
	ofBaseApp::gotMessage(msg);
	
	// mSelf.call("got_message", msg);
}



