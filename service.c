#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <libnotify/notify.h> 
#include <sys/inotify.h>  


#define EXIT_SUCCESS 0
#define EXIT_FAILURE_FEW_ARGS 1
#define EXIT_FAILURE_INOTIFY_INIT 2
#define EXIT_ERROR_WATCH_EVENT 3
#define EXIT_ERROR_BASE_PATH 4
#define EXIT_READ_INOTIFY 5
#define EXIT_LIBNOTIFY_INIT 6

int event_queue = -1;
int event_status = -1;

char Program_Title = "File_Monitoring";

void signal_handler(int signal) {
  int closeStatus = -1;

  printf("Signal received, cleaning up...\n");
  closeStatus = inotify_rm_watch(event_queue, IeventStatus);
  if(closeStatus == -1) {
    fprintf(stderr, "Error removing from watch queue!\n");
  }
  close(event_queue)
  exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{
  bool libnotifyInitStatus = false;

  char *base_Path = NULL;
  char *token = NULL;
  char *notification_message = NULL;
 
  NotifyNotification *notifyHandle; 

  char buffer[4096];
  int read_length;
  const struct  inotify_event* watch_Event;

  const uint32_t watch_mask = IN_ACCESS | IN_DELETE | IN_CREATE | IN_MODIFY | IN_CLOSE_WRITE | IN_MOVE_SELF;
  if(argc < 2) {
    fprintf(stderr, "USAGE: service PATH\n");
    exit(EXIT_FAILURE_FEW_ARGS);
  }

  base_Path = (char*)malloc(strlen(argv[1]) + 1);

  strcpy(base_Path, argv[1]);

  token = strtok(base_Path,"/");

  while (token != NULL) {
    base_Path = token;
    token = strtok(NULL, "/");
  }

  if(base_Path == NULL) {
    fprintf(stderr, "error getting base path \n");
    exit(4);
  }
   
  libnotifyInitStatus = notify_init(Program_Title);
  if(!libnotifyInitStatus) {
    fprintf(stderr, "Error initialising lib_notify");
    exit(6);
  }

  event_queue = inotify_init();
  if(event_queue == -1) {
    fprintf(stderr, "Error initialising inotify instance\n");
    exit(2);
  }

  event_status = inotify_add_watch(event_queue, argv[1], watch_mask);   
  if(event_status == -1) {
    fprintf(stderr, "Error adding file to watch_event\n");
    exit(3);
  }

  signal(SIGABRT, signal_handler);
  signal(SIGINT ,signal_handler);
  signal(SIGTERM, signal_handler);
  
  while(true) {
    printf("Waiting for event...... \n");

    read_length = read(event_queue, buffer, sizeof(buffer));  
    if(read_length == -1) {
      fprintf(stderr, "Error reading from inotify instance\n");
      exit(5);
    }

    for(char* buff_pointer = buffer; buff_pointer < buffer + read_length; buff_pointer += sizeof(struct inotify_event) + watch_Event->len ) {
        
      watch_Event = (const struct inotify_event *) buff_pointer;
      notification_message = NULL;
      
      if(watch_Event->mask & IN_MODIFY) {
        notification_message = "FILE MODIFIED \n";
      }

      if(watch_Event->mask & IN_CREATE) {
        notification_message = "FILE CREATED\n";
      }

      if(watch_Event->mask & IN_ACCESS) {
        notification_message = "FILE_ACCESSED\n";
      }

      if(watch_Event->mask & IN_DELETE) {
        notification_message = "FILE DELETED \n";
      }

      if(watch_Event->mask & IN_MOVE_SELF) {
        notification_message = "FILE MOVED\n";
      }

      if(watch_Event->mask & IN_CLOSE_WRITE) {
        notification_message = "FILE_WRITTEN_CLOSED\n";
      }

      if(notification_message == NULL) {
        continue;
      }
      
      notifyHandle = notify_notification_new(base_Path, notification_message, "dialog-information");  
      if(notifyHandle == NULL) {
        fprintf(stderr, "notification handlw was null! \n");
        continue;
      }
      
      notify_notification_show(notifyHandle, NULL);
      

    }

  }

  
}
