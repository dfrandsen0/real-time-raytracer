import javax.swing.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.nio.MappedByteBuffer;

// This file implements user input. Each time a user types a command at the keyboard,
//   it is added to the sync file and sent over all at once on frame change.
//
// Controls:
//   W: Forward
//   S: Backward
//   A: Strafe Left
//   D: Strafe Right
//   E: Turn Right
//   Q: Turn Left

public class KeyListenerObject implements KeyListener {
    private final int MAX_COMMANDS = 12;
    byte[] keyArray;
    int keyIndex;
    boolean wdown = false;
    boolean sdown = false;
    boolean adown = false;
    boolean ddown = false;
    boolean edown = false;
    boolean qdown = false;

    public KeyListenerObject() {
        keyArray = new byte[MAX_COMMANDS];
        keyIndex = 0;
    }

    @Override
    public void keyTyped(KeyEvent e) {}

    @Override
    public synchronized void keyPressed(KeyEvent e) {
        byte code = (byte)e.getKeyCode();

        if(code == 65) {
            adown = true;
        } else if(code == 68) {
            ddown = true;
        } else if(code == 87) {
            wdown = true;
        } else if(code == 83) {
            sdown = true;
        } else if(code == 69) {
            edown = true;
        } else if(code == 81) {
            qdown = true;
        } else {
            return;
        }

        keyArray[keyIndex] = code;
        keyIndex++;
    }

    @Override
    public synchronized void keyReleased(KeyEvent e) {
        byte code = (byte)e.getKeyCode();

        if(code == 65) {
            adown = false;
        } else if(code == 68) {
            ddown = false;
        } else if(code == 87) {
            wdown = false;
        } else if(code == 83) {
            sdown = false;
        } else if(code == 69) {
            edown = false;
        } else if(code == 81) {
            qdown = false;
        }

    }

    public synchronized void flip(MappedByteBuffer mbb, int start) {
        int origPosition = mbb.position();
        mbb.position(start + 3);
        mbb.put(keyArray);
        mbb.force();

        mbb.position(origPosition);

        keyIndex = 0;
        if(wdown) {
            keyArray[keyIndex] = 87;
            keyIndex++;
        }

        if(sdown) {
            keyArray[keyIndex] = 83;
            keyIndex++;
        }

        if(adown) {
            keyArray[keyIndex] = 65;
            keyIndex++;
        }

        if(ddown) {
            keyArray[keyIndex] = 68;
            keyIndex++;
        }

        if(edown) {
            keyArray[keyIndex] = 69;
            keyIndex++;
        }

        if(qdown) {
            keyArray[keyIndex] = 81;
            keyIndex++;
        }

        for(int i = keyIndex; i < MAX_COMMANDS; ++i) {
            keyArray[i] = 0;
        }
    }
}
