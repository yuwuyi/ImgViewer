#ifndef KeyHandle_h__
#define KeyHandle_h__

class QKeyEvent;

class KeyHandle {
public:
	KeyHandle();

	void keyPressEvent ( QKeyEvent * event );
	void keyReleaseEvent ( QKeyEvent * event );

};

#endif // KeyHandle_h__