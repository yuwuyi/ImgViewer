#ifndef IMGVIEWER_H
#define IMGVIEWER_H

#include <QtGui/QMainWindow>
#include "ui_imgviewer.h"

class ImgViewer : public QMainWindow
{
	Q_OBJECT

public:
	ImgViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ImgViewer();

	public slots:
		void loadImages();
		void loadCM();
		void LoadMesh();
		void sliceDir(int);
		void slice(int);
		void updateSliceInfo();
public:
	Ui::ImgViewerClass ui;
};

#endif // IMGVIEWER_H
