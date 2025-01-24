import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.swing.*;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

// This file runs the front end for the ray tracer. It initializes itself and waits for the
//   backend. When the backend signals through syncFile, it draws a buffer to the screen.


public class Main {
    public static void main(String[] args) throws IOException, InterruptedException {
        final int MAX_OBJECT = 786432;

        //one day, I may replace with a socket. Signals are out of the question because of the JVM
        RandomAccessFile fileSync = new RandomAccessFile("syncFile.txt", "rw");
        FileChannel channelSync = fileSync.getChannel();
        MappedByteBuffer mbbsync = channelSync.map(FileChannel.MapMode.READ_WRITE, 0, channelSync.size());

        RandomAccessFile file1 = new RandomAccessFile("sharedFile1.txt", "r");
        FileChannel channel1 = file1.getChannel();
        MappedByteBuffer mbb1 = channel1.map(FileChannel.MapMode.READ_ONLY, 0, channel1.size());

        RandomAccessFile file2 = new RandomAccessFile("sharedFile2.txt", "r");
        FileChannel channel2 = file2.getChannel();
        MappedByteBuffer mbb2 = channel2.map(FileChannel.MapMode.READ_ONLY, 0, channel2.size());

        int mbbsyncStart = mbbsync.position();
        int mbb1Start = mbb1.position();
        int mbb2Start = mbb2.position();

        JFrame frame = new JFrame();
        JLabel label = new JLabel();
        frame.setSize(512, 512);

        frame.getContentPane().add(label, BorderLayout.CENTER);
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        BufferedImage theImage = new BufferedImage(512, 512, BufferedImage.TYPE_3BYTE_BGR);
        ImageIcon icon = new ImageIcon(theImage);
        label.setIcon(icon);
        frame.setVisible(true);

        KeyListenerObject listener = new KeyListenerObject();
        label.addKeyListener(listener);
        label.grabFocus();
        label.requestFocus();

	// Sleep the program for a second to make sure that the window recieves focus and is completely
	//   finished setup.
        Thread.sleep(1000);
        System.out.println("Ready for Backend.");


        byte[] valueArray = new byte[MAX_OBJECT];
        byte[] imgData = ((DataBufferByte)theImage.getRaster().getDataBuffer()).getData();
        long startTime = System.nanoTime();
        long endTime = System.nanoTime();


        int buffer;
        while(true) {
            startTime = endTime;
            endTime = System.nanoTime();
            System.out.print("Frame: ");
            System.out.println(endTime - startTime);

            while(true) {
                mbbsync.position(mbbsyncStart);

                //0 position
                byte value = mbbsync.get();
                if(value == 1) {
                    mbbsync.position(mbbsyncStart);
                    //0 position
                    mbbsync.put((byte)0);
                    mbbsync.load();

                    //1 position
                    buffer = mbbsync.get();
                    break;
                }

                //wait in microseconds
                Thread.sleep(1);
            }

            if(buffer == 1) {
                mbb1.get(valueArray);
                System.arraycopy(valueArray, 0, imgData, 0, valueArray.length);
                label.repaint();
                mbb1.position(mbb1Start);
            } else {
                mbb2.get(valueArray);
                System.arraycopy(valueArray, 0, imgData, 0, valueArray.length);
                label.repaint();
                mbb2.position(mbb2Start);
            }

            listener.flip(mbbsync, mbbsyncStart);
            //write changes
            //switch pipelines
            //2 position
            mbbsync.put((byte)1);
            mbbsync.force();
        }
    }
}

