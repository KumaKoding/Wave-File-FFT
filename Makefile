.SILENT:


all:
	gcc src/main.c src/decode_wav.c src/FFT.c src/plot.c src/encode_bmp.c -Wall -fsanitize=undefined

a.out: 
	gcc src/main.c src/decode_wav.c src/FFT.c src/plot.c src/encode_bmp.c -Wall -fsanitize=undefined

watermelon1.bmp: a.out
	./a.out watermelon1.wav watermelon1.bmp
	mv sound.bmp watermelon1Sound.bmp

watermelon2.bmp: a.out
	./a.out watermelon2.wav watermelon2.bmp
	mv sound.bmp watermelon2Sound.bmp

convert-images: watermelon1.bmp watermelon2.bmp
	ffmpeg -y -loglevel quiet -i watermelon1.bmp watermelon1.png
	ffmpeg -y -loglevel quiet -i watermelon2.bmp watermelon2.png

clean:
	rm a.out watermelon1Sound.bmp watermelon2Sound.bmp watermelon1.bmp watermelon2.bmp watermelon1.png watermelon2.png
