# PhotoPrint

This is a little gui application for a photobox printer. It allows the user to
browse through the images and then select and print a specific image.

## Features
 * Automatic detection of new Images
 * Copy images to local directory (usefull for slow network drives)
 * Fullscreen view
 * optimized for touch displays
 * Configurable thumbnail size
 * Autostart
 * prepared for executing external sripts (control LED's, play sounds, etc.)


# Build

To build the Project you need the QT framework 


## Linux



``` shell
sudo apt install qtbase5-dev qt5-qmake

git clone https://github.com/BrainHunter/PhotoPrint.git
cd PhotoPrint
git submodule update --init --recursive

```


```
 qmake Photobox_Widget.pro
 make
 ./Photobox_Widget
```