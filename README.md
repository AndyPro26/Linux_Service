# Linux_Service
Linux service that watches a file on a system and send a message on a desktop if that file is accessed or change. 
A pop up will be displayed on desktop window saying this file was modified/accessed.

# Notification API's used
1. libnotify : https://developer-old.gnome.org/libnotify/unstable/libnotify-notify.html :  notification api
2. inotify : https://man7.org/linux/man-pages/man7/inotify.7.html  : monitonring file system events
