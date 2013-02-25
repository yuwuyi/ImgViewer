#include "KeyHandle.h"
#include <QKeyEvent>

// 
// void KeyHandle::keyPressEvent ( QKeyEvent * event ) {
// 	if (event->modifiers() & Qt::ControlModifier) {
// 		isCtrl = true;
// 	}
// 	std::cout << isCtrl << "\n";
// 
// 	if (snapshots.size() > 1) {
// 		if (event->key() == Qt::Key_Delete) {
// 			currentSnapshot->clear();
// 			snapshots.erase(snapshots.begin() + currentSnapshot->id);
// 			for (int sid = 0; sid < snapshots.size(); ++sid) {
// 				snapshots[sid]->id = sid;
// 			}
// 			pickingSnapShot = currentSnapshot->id -1;
// 			if (pickingSnapShot < 0) {
// 				pickingSnapShot = 0;
// 			}
// 			delete currentSnapshot;
// 			currentSnapshot = snapshots[pickingSnapShot];
// 
// 
// 			slicepart = currentSnapshot->slice_part;
// 			slicedir = currentSnapshot->slice_dir;
// 
// 			emit (triggerUpdateSliceInfo());
// 		}
// 	}
// 
// 	updateGL();
// 
// }
// 
// void KeyHandle::keyReleaseEvent ( QKeyEvent * event ) {
// 	//if (event->modifiers() & Qt::ControlModifier) {
// 	isCtrl = false;
// 	//}
// 	std::cout << isCtrl << "\n";
// }
// 
// 
// 
