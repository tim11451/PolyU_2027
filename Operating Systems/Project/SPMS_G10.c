#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_PARKING 3 // The number of parking spaces N
#define MAX_ESSENTIALS 3 //The number of each pair of essentials
#define MAX_MEMBERS 5 //The number of members
#define MAX_LENGTH 1024
#define MAX_ESSENTIAL_LENGTH 50

typedef enum {
    EVENT,
    RESERVATION,
    PARKING,
    ESSENTIAL
} BookingType;

typedef struct {
    BookingType type;
    char member[50];
    char date[11]; // YYYY-MM-DD
    char time[6]; // 24H
    int duration;
    char essentials[MAX_ESSENTIALS][MAX_ESSENTIAL_LENGTH];
    int essentials_count; // assume for every reservation: 3 essentials
    bool accepted; 
    int parking_first_available_slot; //  -1 -> unavailable; 0,1,2 -> The first available parking space slot (If MAX_PARKING = 3)
    int essentials_first_available_slot[3]; // At most 3 pairs of essentials for each booking; slot[i] -> similar meaning as above
} Booking;
//FCFS
Booking accepted_Bookings[MAX_MEMBERS][100];
Booking rejected_Bookings[MAX_MEMBERS][100]; 

int accepted_Booking_count [MAX_MEMBERS]= {0};
int rejected_Booking_count [MAX_MEMBERS]= {0};

Booking Parking_Timetable[MAX_PARKING][7][24]; 
Booking b_c_Timetable[MAX_ESSENTIALS][7][24] ; 
Booking l_u_Timetable[MAX_ESSENTIALS][7][24] ;
Booking i_v_Timetable[MAX_ESSENTIALS][7][24] ; 

//Priority
Booking priority_accepted_Bookings[MAX_MEMBERS][100];
Booking priority_rejected_Bookings[MAX_MEMBERS][100]; 

int priority_accepted_Booking_count [MAX_MEMBERS]= {0};
int priority_rejected_Booking_count [MAX_MEMBERS]= {0};

Booking priority_Parking_Timetable[MAX_PARKING][7][24]; 
Booking priority_b_c_Timetable[MAX_ESSENTIALS][7][24];
Booking priority_l_u_Timetable[MAX_ESSENTIALS][7][24];
Booking priority_i_v_Timetable[MAX_ESSENTIALS][7][24];

int totalBookings_FCFS;
int totalBookings_Pri;

//Funtion Declaration
void addbooking(Booking booking);
void bookEssentials(Booking booking);
int isParkingAvailable(Booking booking);
int isEssentialAvailable(Booking booking, int n);
Booking child_process(char *input);
void parent_process(Booking booking);
void handleBooking(char *input);
void printSchedule();

void priority_addbooking(Booking booking);
void priority_bookEssentials(Booking booking);
int isParkingPriorityAvailable(Booking booking);
int isEssentialPriorityAvailable(Booking booking, int n);
Booking priority_child_process(char *input);
void priority_parent_process(Booking booking);
void priority_handleBooking(char *input);
void cancelParking(Booking booking);
void priority_printSchedule();

char *getInput();
const char* BookingTypeToString(BookingType type);
int BookingToPriority(char * cmd);
void read_batch_file(const char* filename);

void handleBooking(char* input){
    // pipes[0] -> parent to child 
    // pipes[1] -> child to parent
    int pipes[2][2]; 
    char line[MAX_LENGTH];
    Booking booking;
    pid_t pid;
    totalBookings_FCFS++;
    int i = 0, n = 0, m=0;
        if(pipe(pipes[0])<0||pipe(pipes[1])<0){
            printf("Pipe creation error\n"); 
            exit(1);  
        }
        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } 
        else if (pid == 0) {
            // child process
            close(pipes[0][1]); 
            close(pipes[1][0]); 

            if ((n = read(pipes[0][0],line,MAX_LENGTH) > 0)) {
                printf("<child> <%d> message [%s] received\n",getpid(),line);
                booking=child_process(line);
                write(pipes[1][1],&booking,sizeof(booking));
            }
            close(pipes[0][0]);
            close(pipes[1][1]);
            exit(0);
        }
        else{ // parent process
            close(pipes[0][0]);
            close(pipes[1][1]);
            strcpy(line, input); 
            write(pipes[i][1], line, MAX_LENGTH); 
            if ((m = read(pipes[1][0],&booking,sizeof(booking)) > 0)) { 
                // printf("<parent> message received\n");
                parent_process(booking);
            }
            close(pipes[0][1]);
            close(pipes[i][1]);

        }
    wait(NULL);

}

void priority_handleBooking(char* input){
    int pipes[2][2];
    char line[MAX_LENGTH];
    Booking booking;
    pid_t pid;

    int i = 0, n = 0, m=0;

  
        if(pipe(pipes[0])<0||pipe(pipes[1])<0){
            printf("Pipe creation error\n"); 
            exit(1);  
        }

        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } 
        else if (pid == 0) {
            close(pipes[0][1]);
            close(pipes[1][0]);

            if ((n = read(pipes[0][0],line,MAX_LENGTH) > 0)) {
                printf("<priority> <child> <%d> message [%s] received\n",getpid(),line);
                booking=priority_child_process(line);
                write(pipes[1][1],&booking,sizeof(booking));
            }
            close(pipes[0][0]);
            close(pipes[1][1]);
            exit(0);
        }
        else{
            close(pipes[0][0]);
            close(pipes[1][1]);
            strcpy(line, input);
            write(pipes[0][1], line, MAX_LENGTH);
            if ((m = read(pipes[1][0],&booking,sizeof(booking)) > 0)) { 
                // printf("<parent> message received\n");
                priority_parent_process(booking);
            }
            close(pipes[0][1]);
            close(pipes[0][1]);

        }
    

    
    wait(NULL);

}

Booking child_process(char* input) {
    Booking booking;
    char command[20];
    char essentials[MAX_LENGTH] = "";
    char duration[5];
    booking.accepted = false;

    sscanf(input, "%s %s %s %s %s %[^\n]",
           command,
           booking.member,
           booking.date,
           booking.time,
           duration,
           essentials);
    
    if (strcmp(command, "addParking") == 0) booking.type = PARKING;
    else if (strcmp(command, "addReservation") == 0) booking.type = RESERVATION;
    else if (strcmp(command, "addEvent") == 0) booking.type = EVENT;
    else if (strcmp(command, "bookEssentials") == 0) booking.type = ESSENTIAL;

    booking.duration = atoi(duration);

    booking.parking_first_available_slot = -1;
    int i;
    for (i = 0; i < 3;i++){
        booking.essentials_first_available_slot[i] = -1;
    }

    char *token;
    int count = 0;
    token = strtok(essentials, " ");
    while (token != NULL && count < MAX_ESSENTIALS) {
        strncpy(booking.essentials[count], token, MAX_ESSENTIAL_LENGTH - 1);
        booking.essentials[count++][MAX_ESSENTIAL_LENGTH - 1] = '\0';
        token = strtok(NULL, " ");
    }
    booking.essentials_count = count;

    booking.parking_first_available_slot = isParkingAvailable(booking);
    int cur;
    for (cur = 0; cur < booking.essentials_count; cur++) {
        booking.essentials_first_available_slot[cur] = isEssentialAvailable(booking, cur);
    }

    return booking;
}

Booking priority_child_process(char* input) {
    Booking booking;
    char command[20];
    char essentials[MAX_LENGTH] = "";

    char duration[5];
    booking.accepted = false;

    sscanf(input, "%s %s %s %s %s %[^\n]",
           command,
           booking.member,
           booking.date,
           booking.time,
           duration,
           essentials);
    
    if (strcmp(command, "addParking") == 0) booking.type = PARKING;
    else if (strcmp(command, "addReservation") == 0) booking.type = RESERVATION;
    else if (strcmp(command, "addEvent") == 0) booking.type = EVENT;
    else if (strcmp(command, "bookEssentials") == 0) booking.type = ESSENTIAL;

    booking.duration = atoi(duration);

    booking.parking_first_available_slot = -1;
    int i;
    for (i = 0; i < 3;i++){
        booking.essentials_first_available_slot[i] = -1;
    }

    char *token;
    int count = 0;
    token = strtok(essentials, " ");
    while (token != NULL && count < MAX_ESSENTIALS) {
        strncpy(booking.essentials[count], token, MAX_ESSENTIAL_LENGTH - 1);
        booking.essentials[count++][MAX_ESSENTIAL_LENGTH - 1] = '\0';
        token = strtok(NULL, " ");
    }
    booking.essentials_count = count;

    booking.parking_first_available_slot = isParkingPriorityAvailable(booking);
    int cur;
    for (cur = 0; cur < booking.essentials_count; cur++) {
        booking.essentials_first_available_slot[cur] = isEssentialPriorityAvailable(booking, cur);
    }

    return booking;
}

void parent_process(Booking booking){
    if (booking.type == ESSENTIAL) {
        
        int i;
        for (i = 0; i < booking.essentials_count; i++) {
            if (booking.essentials_first_available_slot[i] == -1) {
                booking.accepted = false;
                char member_id;
                sscanf(booking.member, "-member_%c", &member_id);
                rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking;
                // printf("---rejected\n");
                return;
            }
        }
        booking.accepted = true;
        bookEssentials(booking);
    } 
    else {
        
        if (booking.parking_first_available_slot == -1) {
            booking.accepted = false;
            char member_id;
            sscanf(booking.member, "-member_%c", &member_id);
            // printf("---rejected\n");
            rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking; // error member_id-'A'
            return;
        }
        int i;
        for (i = 0; i < booking.essentials_count; i++) {
            if (booking.essentials_first_available_slot[i] == -1) {
                booking.accepted = false;
                char member_id;
                sscanf(booking.member, "-member_%c", &member_id);
                rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking;
                // printf("---rejected\n");
                return;
            }
        }
        if (booking.essentials_count == 1 && strcmp(booking.essentials[0], "") == 0) {
            booking.accepted = true;
            addbooking(booking);
        } else {
            int i;
            for (i = 0; i < booking.essentials_count; i++) {
                if (booking.essentials_first_available_slot[i] == -1) {
                    booking.accepted = false;
                    char member_id;
                    sscanf(booking.member, "-member_%c", &member_id);
                    rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking;
                    // printf("---rejected\n");
                    return;
                }
            }
            booking.accepted = true;

            addbooking(booking);
            bookEssentials(booking);
            }       
        }
    
    if (booking.accepted) {
        // printf("---accepted\n");
    } 
    
    return;
    }

void priority_parent_process(Booking booking){
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    int i = 0, j = 0,n = 0,a = 0,b = 0,c = 0;
    totalBookings_Pri++;

    if (booking.type == ESSENTIAL) {
        int i;
        for (i = 0; i < booking.essentials_count; i++) {
            if (booking.essentials_first_available_slot[i] == -1) {
                booking.accepted = false;
                char member_id;
                sscanf(booking.member, "-member_%c", &member_id);
                priority_rejected_Bookings[member_id-'A'][priority_rejected_Booking_count[member_id-'A']++] = booking;
                // printf("---rejected\n");
                return;
            }
        }    
        booking.accepted = true;
        priority_bookEssentials(booking);
    } 
    else {
        if (booking.parking_first_available_slot == -1) {
            booking.accepted = false;
            char member_id;
            sscanf(booking.member, "-member_%c", &member_id);
            // printf("---rejected\n");
            priority_rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking; 
            return;
        }
        int i;
        for (i = 0; i < booking.essentials_count; i++) {
            if (booking.essentials_first_available_slot[i] == -1) {
                booking.accepted = false;
                char member_id;
                sscanf(booking.member, "-member_%c", &member_id);
                rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking;
                // printf("---rejected\n");
                return;
            }
        }
        for(i=atoi(booking.time);i<atoi(booking.time)+booking.duration;i++){
            if( strcmp(priority_Parking_Timetable[booking.parking_first_available_slot][index][i].member,"")!=0 &&
                priority_Parking_Timetable[booking.parking_first_available_slot][index][i].accepted) {
                cancelParking(priority_Parking_Timetable[booking.parking_first_available_slot][index][i]);
            }
        }
        for (i = 0; i < booking.essentials_count; i++) {
        if ((strcmp(booking.essentials[i], "battery") == 0) || (strcmp(booking.essentials[i], "cable") == 0)) {
            for(j=atoi(booking.time);j<atoi(booking.time)+booking.duration;j++){
                if(strcmp(priority_b_c_Timetable[booking.essentials_first_available_slot[i]][index][j].member,"")!=0 &&
                priority_b_c_Timetable[booking.essentials_first_available_slot[i]][index][j].accepted) {
                cancelParking(priority_b_c_Timetable[booking.essentials_first_available_slot[i]][index][j]);
                }
            }
        }
        else if ((strcmp(booking.essentials[n], "lockers") == 0) || (strcmp(booking.essentials[n], "umbrella") == 0)) {
            for(j=atoi(booking.time);j<atoi(booking.time)+booking.duration;j++){
                if(strcmp(priority_l_u_Timetable[booking.essentials_first_available_slot[i]][index][j].member,"")!=0 &&
                priority_l_u_Timetable[booking.essentials_first_available_slot[i]][index][j].accepted) {
                cancelParking(priority_l_u_Timetable[booking.essentials_first_available_slot[i]][index][j]);
                }
            }
        }
        else if ((strcmp(booking.essentials[n], "inflationservice") == 0) || (strcmp(booking.essentials[n], "valetparking") == 0)) {
            for(j=atoi(booking.time);j<atoi(booking.time)+booking.duration;j++){
                if(strcmp(priority_i_v_Timetable[booking.essentials_first_available_slot[i]][index][j].member,"")!=0 &&
                priority_i_v_Timetable[booking.essentials_first_available_slot[i]][index][j].accepted) {
                cancelParking(priority_i_v_Timetable[booking.essentials_first_available_slot[i]][index][j]);
                }
            }
        }} 

        if (booking.essentials_count == 1 && strcmp(booking.essentials[0], "") == 0) {
            booking.accepted = true;
            totalBookings_Pri++;
            priority_addbooking(booking);
        } else {
            int i;
            for (i = 0; i < booking.essentials_count; i++) {
                if (booking.essentials_first_available_slot[i] == -1) {
                    booking.accepted = false;
                    char member_id;
                    sscanf(booking.member, "-member_%c", &member_id);
                    rejected_Bookings[member_id-'A'][rejected_Booking_count[member_id-'A']++] = booking;
                    // printf("---rejected\n");
                    return;
                }
            }
            booking.accepted = true;
            priority_addbooking(booking);
            priority_bookEssentials(booking);
            }       
        }
    if (booking.accepted) {
        // printf("---accepted\n");
    } 

    return;
}
void cancelParking(Booking booking){
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10; 
    int i;
    char member_id;
    sscanf(booking.member, "-member_%c", &member_id);
    int member_index = member_id - 'A';

    for (i = atoi(booking.time); i < atoi(booking.time) +  booking.duration; i++) {
        priority_Parking_Timetable[booking.parking_first_available_slot][index][i].accepted = false; 
    }
    int a=0,b=0,c=0,n=0;
    for(n=0;n<booking.essentials_count;n++){
    if ((strcmp(booking.essentials[n], "battery") == 0) || (strcmp(booking.essentials[n], "cable") == 0)) {
        for(i=atoi(booking.time);i<atoi(booking.time)+ booking.duration;i++){
            priority_b_c_Timetable[booking.essentials_first_available_slot[n]][index][i].accepted=false;
        }  
    } else if ((strcmp(booking.essentials[n], "lockers") == 0) || (strcmp(booking.essentials[n], "umbrella") == 0)) {
        for(i=atoi(booking.time);i<atoi(booking.time)+ booking.duration;i++){
            priority_l_u_Timetable[booking.essentials_first_available_slot[n]][index][i].accepted=false;
            }
    } else if ((strcmp(booking.essentials[n], "inflationservice") == 0) || (strcmp(booking.essentials[n], "valetparking") == 0)) {
        for(i=atoi(booking.time);i<atoi(booking.time)+ booking.duration;i++){
            priority_i_v_Timetable[booking.essentials_first_available_slot[n]][index][i].accepted=false;
        } 
    } else {
        return ;
    
    }
}
    
    // found the booking to be canceled in accept_Bookings and cancel it
    for (i = 0; i < priority_accepted_Booking_count[member_index]; i++) {
        if (priority_accepted_Bookings[member_index][i].type == booking.type &&
            strcmp(priority_accepted_Bookings[member_index][i].member, booking.member) == 0 &&
            strcmp(priority_accepted_Bookings[member_index][i].date, booking.date) == 0 &&
            strcmp(priority_accepted_Bookings[member_index][i].time, booking.time) == 0 &&
            priority_accepted_Bookings[member_index][i].duration == booking.duration &&
            sizeof(priority_accepted_Bookings[member_index][i].essentials)==sizeof(booking.essentials) &&
            priority_accepted_Bookings[member_index][i].essentials_count==booking.essentials_count &&
            priority_accepted_Bookings[member_index][i].parking_first_available_slot==booking.parking_first_available_slot
        ){
        priority_accepted_Bookings[member_index][i].accepted=false; 
        
        priority_accepted_Booking_count[member_index]-=1;
        priority_rejected_Bookings[member_index][priority_rejected_Booking_count[member_index]] = priority_accepted_Bookings[member_index][i];
        priority_rejected_Booking_count[member_index]+=1;
        break; 
        }
    }
    printf("%s's booking is canceled.\n",booking.member);
}
void addbooking(Booking booking) {
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;

    char member_id;
    sscanf(booking.member, "-member_%c", &member_id);
    accepted_Bookings[member_id-'A'][accepted_Booking_count[member_id-'A']++] = booking;

    int i = 0;
    for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
        Parking_Timetable[booking.parking_first_available_slot][index][i] = booking;
    }
}
void priority_addbooking(Booking booking) {
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;

    char member_id;
    sscanf(booking.member, "-member_%c", &member_id);
    priority_accepted_Bookings[member_id-'A'][priority_accepted_Booking_count[member_id-'A']++] = booking;
    
    int i = 0;
    for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
        priority_Parking_Timetable[booking.parking_first_available_slot][index][i] = booking;
    }
}

void bookEssentials(Booking booking) {
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    
    if(booking.type==ESSENTIAL){
        char member_id;
        sscanf(booking.member, "-member_%c", &member_id);
        accepted_Bookings[member_id-'A'][accepted_Booking_count[member_id-'A']++] = booking;
    }

    int n = 0, i = 0;
    for (n = 0; n < booking.essentials_count;n++){
        if((strcmp(booking.essentials[n], "battery")== 0)||(strcmp(booking.essentials[n], "cable") == 0)){
            for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
                b_c_Timetable[booking.essentials_first_available_slot[n]][index][i] = booking;
            }
        }
        else if((strcmp(booking.essentials[n], "lockers")== 0)||(strcmp(booking.essentials[n], "umbrella") == 0)){
            for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
                l_u_Timetable[booking.essentials_first_available_slot[n]][index][i] = booking;
            }
        }
        else if((strcmp(booking.essentials[n], "inflationservice")== 0)||(strcmp(booking.essentials[n], "valetparking") == 0)){
            for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
                i_v_Timetable[booking.essentials_first_available_slot[n]][index][i] = booking;
            }
        }
    }

}

void priority_bookEssentials(Booking booking) {
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    
    if(booking.type==ESSENTIAL){
        char member_id;
        sscanf(booking.member, "-member_%c", &member_id);
        priority_accepted_Bookings[member_id-'A'][priority_accepted_Booking_count[member_id-'A']++] = booking;
    }

    int n = 0, i = 0;
    for (n = 0; n < booking.essentials_count;n++){
        if((strcmp(booking.essentials[n], "battery")== 0)||(strcmp(booking.essentials[n], "cable") == 0)){
            for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
                priority_b_c_Timetable[booking.essentials_first_available_slot[n]][index][i] = booking;
            }
        }
        else if((strcmp(booking.essentials[n], "lockers")== 0)||(strcmp(booking.essentials[n], "umbrella") == 0)){
            for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
                priority_l_u_Timetable[booking.essentials_first_available_slot[n]][index][i] = booking;
            }
        }
        else if((strcmp(booking.essentials[n], "inflationservice")== 0)||(strcmp(booking.essentials[n], "valetparking") == 0)){
            for (i = atoi(booking.time); i < atoi(booking.time) + booking.duration;i++){
                priority_i_v_Timetable[booking.essentials_first_available_slot[n]][index][i] = booking;
            }
        }
    }

}


int isParkingAvailable(Booking booking) {
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    int i = 0, j = 0;
    int first_available_slot = -1; //Initialization
    
        for (i = 0; i < MAX_PARKING; i++) { 
            bool isfound = true; 
            for (j = atoi(booking.time); j < atoi(booking.time) + booking.duration; j++) {
                if (strcmp(Parking_Timetable[i][index][j].member, "")!=0) {
                    isfound = false; 
                    booking.accepted = false;
                    break; 
                }
            }
            if (isfound) {
                first_available_slot = i; 
                break; 
            }
        }
    return first_available_slot;
}


int isParkingPriorityAvailable(Booking booking){
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    int i = 0, j = 0;
    int first_available_slot = -1;
    for (i = 0; i < MAX_PARKING; i++) { 
        bool isfound = true;
        for (j = atoi(booking.time); j < atoi(booking.time) + booking.duration; j++) { 
            if (strcmp(priority_Parking_Timetable[i][index][j].member,"")!=0 &&
                priority_Parking_Timetable[i][index][j].accepted) {
        
                if (booking.type >= priority_Parking_Timetable[i][index][j].type) {
                    isfound = false;
                    booking.accepted = false;
                    break; 
                } 
                else if(priority_Parking_Timetable[i][index][j].accepted){
                    isfound=true;
                    break;
                }
            }

        }
         if (isfound) {
            first_available_slot = i; 
            break; 
        }
    }
    return first_available_slot; 
}

int isEssentialAvailable(Booking booking, int n) {
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    int i = 0, j = 0;
    int essential_first_available_slot = -1;
    int a=0,b=0,c=0;
    Booking timetable[MAX_PARKING][7][24]; 
    if ((strcmp(booking.essentials[n], "battery") == 0) || (strcmp(booking.essentials[n], "cable") == 0)) {
        for(a=0;a<MAX_PARKING;a++){
            for(b=0;b<7;b++){
                for(c=0;c<24;c++){
                    timetable[a][b][c]=b_c_Timetable[a][b][c];
                }
            }
        }
    } else if ((strcmp(booking.essentials[n], "lockers") == 0) || (strcmp(booking.essentials[n], "umbrella") == 0)) {
        for(a=0;a<MAX_PARKING;a++){
            for(b=0;b<7;b++){
                for(c=0;c<24;c++){
                    timetable[a][b][c]=l_u_Timetable[a][b][c];
                }
            }
        }
    } else if ((strcmp(booking.essentials[n], "inflationservice") == 0) || (strcmp(booking.essentials[n], "valetparking") == 0)) {
        for(a=0;a<MAX_PARKING;a++){
            for(b=0;b<7;b++){
                for(c=0;c<24;c++){
                    timetable[a][b][c]=i_v_Timetable[a][b][c];
                }
            }
        }
    } else {
        return -1; 
    }
    

    for (i = 0; i < MAX_ESSENTIALS; i++) {
        bool isfound = true;
        for (j = atoi(booking.time); j < atoi(booking.time) + booking.duration; j++) { 
            if (strcmp(timetable[i][index][j].member,"")!=0) {
                isfound = false;
                break; 
            }
        }
        if (isfound) {
            essential_first_available_slot = i; 
            break; 
        }   
    }
    return essential_first_available_slot; 
}

int isEssentialPriorityAvailable(Booking booking, int n){
    int day;
    sscanf(booking.date, "%*d-%*d-%d", &day);
    int index = day - 10;
    int i = 0, j = 0;
    int essential_first_available_slot = -1;
    int a=0,b=0,c=0;
    Booking timetable[MAX_PARKING][7][24];
    if ((strcmp(booking.essentials[n], "battery") == 0) || (strcmp(booking.essentials[n], "cable") == 0)) {
        for(a=0;a<MAX_PARKING;a++){
            for(b=0;b<7;b++){
                for(c=0;c<24;c++){
                    timetable[a][b][c]=priority_b_c_Timetable[a][b][c];
                }
            }
        }
    } else if ((strcmp(booking.essentials[n], "lockers") == 0) || (strcmp(booking.essentials[n], "umbrella") == 0)) {
        for(a=0;a<MAX_PARKING;a++){
            for(b=0;b<7;b++){
                for(c=0;c<24;c++){
                    timetable[a][b][c]=priority_l_u_Timetable[a][b][c];
                }
            }
        }
    } else if ((strcmp(booking.essentials[n], "inflationservice") == 0) || (strcmp(booking.essentials[n], "valetparking") == 0)) {
        for(a=0;a<MAX_PARKING;a++){
            for(b=0;b<7;b++){
                for(c=0;c<24;c++){
                    timetable[a][b][c]=priority_i_v_Timetable[a][b][c];
                }
            }
        }
    } else {
        return -1; 
    }

    for (i = 0; i < MAX_ESSENTIALS; i++) { 
        bool isfound = true; 
        for (j = atoi(booking.time); j < atoi(booking.time) + booking.duration; j++) {
            if (strcmp(timetable[i][index][j].member,"")!=0 &&
                timetable[i][index][j].accepted) {
        
                if (booking.type >= timetable[i][index][j].type) {
                    isfound = false;
                    break;
                } 
                else if(timetable[i][index][j].accepted){
                    isfound=true;
                    break;
                }
            }
        }
        if (isfound) {
        essential_first_available_slot = i; 
        break;
        }
    }
    return essential_first_available_slot; 

}

char* getInput() {
    char* input = (char*)malloc(MAX_LENGTH * sizeof(char));
    if (input == NULL) {
        printf("failure in memory allocation.\n");
        return NULL;
    }
    printf("Please enter booking: \n");
    if (fgets(input, MAX_LENGTH, stdin) != NULL) {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
    }
    return input;
}

void read_batch_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return;
    }

    char* line = (char*)malloc(MAX_LENGTH * sizeof(char));
    if (line == NULL) {
        printf("Memory allocation failed\n");
        fclose(file);
        return;
    }

    while (fgets(line, MAX_LENGTH, file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0'; // Remove newline character
            line[len - 2] = '\0'; // Remove newline character

        }
        printf("\n-> [Pending]\n");
        handleBooking(line);
        priority_handleBooking(line);
    }

    free(line); 
    fclose(file);
}

int BookingToPriority(char * cmd){
    Booking booking;
    char command[20];
    char essentials[MAX_LENGTH] = "";
    char duration[5];
    booking.accepted = false;

    int priority;
    sscanf(cmd, "%s %s %s %s %s %[^\n]",
           command,
           booking.member,
           booking.date,
           booking.time,
           duration,
           essentials);
    
    if (strcmp(command, "addParking") == 0) priority = 2;
    else if (strcmp(command, "addReservation") == 0) priority = 1;
    else if (strcmp(command, "addEvent") == 0) priority = 0;
    else if (strcmp(command, "bookEssentials") == 0) priority = 3;
    else priority = -1;

    return priority;
}






const char* BookingTypeToString(BookingType type) {
    switch (type) {
        case EVENT: return "EVENT";
        case RESERVATION: return "RESERVATION";
        case PARKING: return "PARKING";
        case ESSENTIAL: return "*";
        default: return "UNKNOWN"; 
    }
}

// Part III: Output Module
void printSchedule() { 
    printf("*** Parking Booking FCFS - ACCEPTED ***\n");
    int i = 0, j = 0, k = 0;
    for (i = 0; i < MAX_MEMBERS; i++) {
        printf("Member %c:\n", i + 'A');
        for (j = 0; j < accepted_Booking_count[i]; j++) {
            Booking cur_booking = accepted_Bookings[i][j];
            printf("Date: %s, Time: %s, Duration: %d hours, Type: %s, Essentials: ",
                   cur_booking.date, cur_booking.time, cur_booking.duration, BookingTypeToString(cur_booking.type));
                   
            for (k = 0; k < cur_booking.essentials_count; k++) {
                printf("%s ", cur_booking.essentials[k]);
            }
            printf("\n");
        }
    }

    printf("\n*** Parking Booking FCFS - REJECTED ***\n");
    for (i = 0; i < MAX_MEMBERS; i++) {
        printf("Member %c:\n", i + 'A');
        for (j = 0; j < rejected_Booking_count[i]; j++) {
            Booking cur_booking = rejected_Bookings[i][j];
            printf("Date: %s, Time: %s, Duration: %d hours, Type: %s, Essentials: ",
                   cur_booking.date, cur_booking.time, cur_booking.duration, BookingTypeToString(cur_booking.type));
                   
            
            for (k = 0; k < cur_booking.essentials_count; k++) {
                printf("%s ", cur_booking.essentials[k]);
            }
            printf("\n");
        }
    }
}
void priority_printSchedule(){
    printf("*** Parking Booking PRIORITY - ACCEPTED ***\n");
    int i = 0, j = 0, k = 0;
    for (i = 0; i < MAX_MEMBERS; i++) {
        printf("Member %c:\n", i + 'A');
        for (j = 0; j < 100; j++) {
            Booking cur_booking = priority_accepted_Bookings[i][j];
            if(cur_booking.accepted){
                printf("Date: %s, Time: %s, Duration: %d hours, Type: %s, Essentials: ",
                   cur_booking.date, cur_booking.time, cur_booking.duration, BookingTypeToString(cur_booking.type));
            for (k = 0; k < cur_booking.essentials_count; k++) {
                printf("%s ", cur_booking.essentials[k]);
            }
            printf("\n");
            }
            
        }
    }

    printf("\n*** Parking Booking PRIORITY - REJECTED ***\n");
    for (i = 0; i < MAX_MEMBERS; i++) {
        printf("Member %c:\n", i + 'A');
        for (j = 0; j < priority_rejected_Booking_count[i]; j++) {
            Booking cur_booking = priority_rejected_Bookings[i][j];
            printf("Date: %s, Time: %s, Duration: %d hours, Type: %s, Essentials: ",
                   cur_booking.date, cur_booking.time, cur_booking.duration, BookingTypeToString(cur_booking.type));
            for (k = 0; k < cur_booking.essentials_count; k++) {
                printf("%s ", cur_booking.essentials[k]);
            }
            printf("\n");
        }
    }
}

// Part IV: Analyzer Module
void generateSummary(int totalBookings, int accepted, int rejected) {
    // Summary for the bookings
    if (totalBookings < 0 || accepted < 0 || rejected < 0) {
        printf("Error: Booking counts cannot be negative.\n");
        return;
    }
    if (accepted + rejected > totalBookings){
        printf("Wrong data input!\n");
        return;
    }
    printf("Total Bookings: %d\n", totalBookings);
    printf("Accepted: %d (%.1f%%)\n", accepted, (float)accepted / totalBookings * 100);
    printf("Rejected: %d (%.1f%%)\n", rejected, (float)rejected / totalBookings * 100);
}
void FCFS_generateSummarySlots(){
    // Resource utilization, including slots, essentials, etc.  Parking_Timetable
    printf("\nResource Utilization:\n");
    int totalHours = MAX_PARKING * 24 * 7;
    int slot,day,hour;
    int occupiedParkingSlots = 0;
    for (slot=0;slot<MAX_PARKING;slot++){ // for every single parking slot
        // for every day in one week
        for(day=0;day<7;day++){
            for(hour=0;hour<24;hour++){
                if (strcmp(Parking_Timetable[slot][day][hour].member,"")!=0){occupiedParkingSlots++;}
            }
        }
    }
    printf("Parking Slots: %.1f%%\n", (float)occupiedParkingSlots / totalHours * 100);
    // for battery and cable 
    int b_c_totalHours =  MAX_ESSENTIALS * 24 * 7;
    int b_c_occupied = 0;
    int pair;
    for (pair=0;pair<MAX_ESSENTIALS;pair++){ // for every pairs
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if (strcmp(b_c_Timetable[pair][day][hour].member,"")!=0){b_c_occupied++;}
            }
        }
    }
    printf("Battery and Cable Pairs: %.1f%%\n", (float)b_c_occupied / b_c_totalHours * 100);
    // for lockers and umbrellas
    int l_u_totalHours = MAX_ESSENTIALS * 24 * 7;
    int l_u_occupied = 0;
    for (pair=0;pair<MAX_ESSENTIALS;pair++){ // for every pairs
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if (strcmp(l_u_Timetable[pair][day][hour].member,"")!=0){l_u_occupied++;}
            }
        }
    }
    printf("Lockers and Umbrellas Pairs: %.1f%%\n", (float)l_u_occupied / l_u_totalHours * 100);
    // for inflation and valet
    int i_v_totalHours = MAX_ESSENTIALS * 24 * 7;
    int i_v_occupied = 0;
    for (pair=0;pair<MAX_ESSENTIALS;pair++){ // for every pairs
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if (strcmp(i_v_Timetable[pair][day][hour].member,"")!=0){i_v_occupied++;}
            }
        }
    }
    printf("Inflation services and Valet Parkings: %.1f%%\n", (float)i_v_occupied / i_v_totalHours * 100);
    
}
void priority_generateSummarySlots(){
    // Resource utilization, including slots, essentials, etc.  Parking_Timetable
    printf("\nResource Utilization:\n");
    int totalHours = MAX_PARKING * 24 * 7;
    int slot,day,hour;
    int occupiedParkingSlots = 0;
    for (slot=0;slot<MAX_PARKING;slot++){ // for every single parking slot
        // for every day in one week
        for(day=0;day<7;day++){
            for(hour=0;hour<24;hour++){
                if (strcmp(priority_Parking_Timetable[slot][day][hour].member,"")!=0){occupiedParkingSlots++;}
            }
        }
    }
    printf("Parking Slots: %.1f%%\n", (float)occupiedParkingSlots / totalHours * 100);

    // for battery and cable 
    int b_c_totalHours =  MAX_ESSENTIALS * 24 * 7;
    int b_c_occupied = 0;
    int pair;
    for (pair=0;pair<MAX_ESSENTIALS;pair++){ // for every pairs
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if (strcmp(priority_b_c_Timetable[pair][day][hour].member,"")!=0){b_c_occupied++;}
            }
        }
    }
    printf("Battery and Cable Pairs: %.1f%%\n", (float)b_c_occupied / b_c_totalHours * 100);
    // for lockers and umbrellas
    int l_u_totalHours = MAX_ESSENTIALS * 24 * 7;
    int l_u_occupied = 0;
    for (pair=0;pair<MAX_ESSENTIALS;pair++){ // for every pairs
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if (strcmp(priority_l_u_Timetable[pair][day][hour].member,"")!=0){l_u_occupied++;}
            }
        }
    }
    printf("Lockers and Umbrellas Pairs: %.1f%%\n", (float)l_u_occupied / l_u_totalHours * 100);
    // for inflation and valet
    int i_v_totalHours = MAX_ESSENTIALS * 24 * 7;
    int i_v_occupied = 0;
    for (pair=0;pair<MAX_ESSENTIALS;pair++){ // for every pairs
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if (strcmp(priority_i_v_Timetable[pair][day][hour].member,"")!=0){i_v_occupied++;}
            }
        }
    }
    printf("Inflation services and Valet Parkings: %.1f%%\n", (float)i_v_occupied / i_v_totalHours * 100);
    
    
}

// // Compare different scheduling algorithms Parking Slots:
void compareAlgorithms(int fcfsAccepted, int prioAccepted) {
    printf("\n*** Algorithm Comparison ***\n");
    printf("FCFS Accepted: %d\n", fcfsAccepted);
    printf("Priority Accepted: %d\n", prioAccepted);
    
    int points_FCFS = 0, points_Prio = 0;
    int slots,day,hour;
    for (slots=0;slots<MAX_PARKING;slots++){
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if(Parking_Timetable[slots][day][hour].type == EVENT) points_FCFS += 4;
                else if(Parking_Timetable[slots][day][hour].type == RESERVATION) points_FCFS += 3;
                else if(Parking_Timetable[slots][day][hour].type == PARKING) points_FCFS += 2;
                else if(Parking_Timetable[slots][day][hour].type == ESSENTIAL) points_FCFS += 1;
            }
        }
    }
    for (slots=0;slots<MAX_PARKING;slots++){
        for (day=0;day<7;day++){
            for (hour=0;hour<24;hour++){
                if(priority_Parking_Timetable[slots][day][hour].type == EVENT) points_Prio += 4;
                else if(priority_Parking_Timetable[slots][day][hour].type == RESERVATION) points_Prio += 3;
                else if(priority_Parking_Timetable[slots][day][hour].type == PARKING) points_Prio += 2;
                else if(priority_Parking_Timetable[slots][day][hour].type == ESSENTIAL) points_Prio += 1;
            }
        }
    }
    if (points_FCFS > points_Prio) {
        printf("FCFS is the more efficient algorithm.\n");
    } else if (points_FCFS < points_Prio) {
        printf("Priority is the more efficient algorithm.\n");
    } 
    else{
        printf("They are the same good.\n");
    }
    return;
}


int main() {
    char input1 [100];
    while (1){
        if(fgets(input1,sizeof(input1),stdin)!=NULL){
             input1[strcspn(input1, "\n")] = '\0';
        }
        if (strcmp(input1,"./SPMS")==0){
            break;
        }
    }

    char line[256];
    char cmd[100];
    char rest[156];
    printf("Welcome to PolyU\n");
    while (1) {
        printf("------------Command Lists------------\n");
        printf("1. addReservation/addEvent/addParking/bookEssentials\n");
        printf("2. importBatch -filePath\n");
        printf("3. printBookings -mode<fcfs/prio/ALL>\n");
        printf("4. generateSummary\n");
        printf("5. endProgram");
        printf("\n");
        printf("\n");
        printf("Enter your booking:\n");
        if (fgets(line, sizeof(line), stdin) != NULL) {
            size_t len = strlen(line);
            if (len>0 && line[len-1] == '\n'){line[len-1] = '\0';}
        
        // Tokenize the input to get the first word and the rest
            sscanf(line, "%s %[^\n]", cmd, rest);
            // differemt cases
            
            if (strcmp(cmd,"addParking")==0||strcmp(cmd,"bookEssentials")==0 ||strcmp(cmd,"addEvent")==0||strcmp(cmd,"addReservation")==0){
                
                char *user_input = line;
                if (user_input != NULL) {
                    totalBookings_FCFS++;
                    printf("\n-> [Pending]\n");
                    handleBooking(user_input);
                    priority_handleBooking(user_input);
                } 
                else {
                    printf("user input is NULL!\n");
                }
            }
            else if (strcmp(cmd,"importBatch")==0){
            char filePath[MAX_LENGTH];
            strcpy(filePath,rest+1);
            read_batch_file(filePath);

            }
            else if (strcmp(cmd,"printBookings")==0){
                if (strcmp(rest, "-fcfs")==0){
                    printSchedule();
                    printf("\n");
                }
                else if (strcmp(rest,"-prio")==0){
                    priority_printSchedule();
                    printf("\n");
                }
                else if (strcmp(rest,"-ALL")==0){
                    printSchedule();
                    printf("\n");
                    priority_printSchedule();
                    printf("\n");
                }
                printf("-> [Done!]\n");
            } 
            else if (strcmp(cmd,"generateSummary")==0){
                int i, accepted=0,rejected=0;
                printf("*** Parking Booking Manager - Summary Report *** \n");
                printf("Performance:\n");
                printf(" For FCFS:\n");
                for(i=0;i<5;i++){
                        accepted += accepted_Booking_count[i];
                    }
                rejected = totalBookings_FCFS - accepted;
                generateSummary(totalBookings_FCFS,accepted,rejected);
                FCFS_generateSummarySlots();
                printf("\n");
                printf (" For Priority:\n");
                int pri_accepted = 0;
                int pri_rejected = 0;
                // calculate the data of priority 
                for (i=0;i<5;i++){
                    pri_accepted += priority_accepted_Booking_count[i];
                }
                pri_rejected = totalBookings_Pri - pri_accepted;
                generateSummary(totalBookings_Pri,pri_accepted,pri_rejected);
                priority_generateSummarySlots();
                compareAlgorithms(accepted,pri_accepted);
            }  
            else if (strcmp(cmd,"endProgram")==0){
                printf("Bye!\n");
                exit(0);
            }
        }
        else{
            printf("Input is NULL!!\n");
        }

    }
    return 0;
}
