#include "../include/simulator.h"
#include <vector>
#include <iostream>
#include <array>
#include <tuple>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/
//Author: Motaz Khalifa
/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
struct pkt sentPacket;


struct pkt receivedPacket;


std::vector<msg> sendbuffer;
std::vector<int> sendbufferstatus;
std::vector<int> Timers(10000,-1);
std::vector<pkt> rcvbuffer(10000, receivedPacket);
std::vector<int> rcvbufferstatus(10000,0);
std::vector<int> prevseqs;
//0 -> not sent
//1 -> sent
//2 -> ACK'ed

//0 -> received
//1 -> ACK'd
//2 -> uploaded


int sendbase = 0;
int rcvbase = 0;
int nextseqnum = 0;
int expectedseqnum = 0;
int windowsize = 0;
/* called from layer 5, passed the data to be sent to other side */
void sendMessage(int index)
{
  std::cout << "SEND MESSAGE() \n";
  struct msg firstmessage = sendbuffer[index];
  std::cout << "Start Parsing \n";
  int checksum = 0;
  for (int i = 0; i < 20; i++){
    std::cout << firstmessage.data[i];
    sentPacket.payload[i] = firstmessage.data[i];  //copying data from message to packet
    checksum = checksum + int(firstmessage.data[i]);    //start calculating checksum
  }
  //perform checksum
  std::cout << "\n";
  sentPacket.seqnum = index;
  std::cout << "seq: ";
  std::cout << sentPacket.seqnum << "\n";
  checksum = checksum + sentPacket.seqnum;
  std::cout << "ack: ";
  std::cout << sentPacket.acknum << "\n";
  checksum = checksum + sentPacket.acknum;
  std::cout << "Checksum + seq + ack: ";
  std::cout << checksum << "\n";
  sentPacket.checksum = checksum;
  tolayer3(0, sentPacket);
  std::cout << "Sending packet to layer 3B.. \n";
  //change to sent
  sendbufferstatus[index] = 1;
  //starting timer
  Timers[index] = 15;
 // starttimer(0,1);
}


void A_output(struct msg message)
{
  std::cout << "A OUTPUT: (1) \n";
  std::cout << "Pushing message to buffer.. \n";
  sendbuffer.push_back(message);
  sendbufferstatus.push_back(0);
  std::cout << "sendbase: ";
  std::cout << sendbase << "\n";
  std::cout << "nextseqnum: ";
  std::cout << nextseqnum << "\n";
  std::cout << "window size: ";
  std::cout << windowsize << "\n";
  std::cout << "actual window size: ";
  std::cout << getwinsize() << "\n";
if (nextseqnum < (sendbase + windowsize)){
  std::cout << "Sender is free.. \n";
  std::cout << "Calling sendMessage().. \n";
  sendMessage(nextseqnum);
  //increment nextseqnum
  std::cout<< "increment nextseqnum.. \n";
  nextseqnum = nextseqnum + 1;
} else {
  std::cout << "Sender is busy.. \n";
}

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  std::cout << "A INPUT: (3) \n";
  bool checked = true;
  int checksum = 0;
  for (int i = 0; i < 20; i++){
      std::cout << packet.payload[i];
      checksum = checksum + int(packet.payload[i]);   //adding up asci values
    }
  checksum = checksum + packet.acknum;
  checksum = checksum + packet.seqnum;
  std::cout << "\n";
  std::cout << "packet.acknum: ";
  std::cout << packet.acknum << "\n";
  std::cout << "packet.seqnum: ";
  std::cout << packet.seqnum << "\n";
  std::cout << "packet.checksum: ";
  std::cout << packet.checksum << "\n";
  std::cout << "Calculated checksum: ";
  std::cout << checksum << "\n"; 
  if (checksum == packet.checksum){
    checked = true;
  } else {
    checked = false;
  }


  if (!(checked)){
    std::cout << "Checksum not equal\n";
    //do nothing
  } else if ((checked) && ((packet.acknum >= sendbase) && (packet.acknum <= (sendbase + windowsize-1)))){
    std::cout << "Checksum equal\n";
    //change to ACK'd
    sendbufferstatus[packet.acknum] = 2;
    //reset timer
    std::cout << Timers[packet.acknum] << "\n";
    Timers[packet.acknum] = -1 ;
    std::cout << Timers[packet.acknum] << "\n";
    if (packet.acknum == sendbase){
      int i = sendbase;
      int x = 0;
      while ((sendbufferstatus[i] == 1)){

      i = i+1;
      }
      //moving sendbase
      sendbase = i + 1;
    }


  }
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
  std::cout << "\n";
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  std::cout << "TIMERRRR \n";
  for (int i =0; i < Timers.size(); i++){
    if (Timers[i] == -1){
      //do nothing
    } else if (Timers[i] == 0) {
      std::cout << "Index of timer: ";
      std::cout << i << "\n";
      sendMessage(i);
      //resend message
    } else {
      Timers[i] = Timers[i] - 1;
      //decrement by one
    }
  }
  starttimer(0, 1);
/*
  std::cout << "\n";
  std::cout << "TIMER INTERRUPT!! \n";
  std::cout << "Resend packets to layer 3B, restart timer \n";
  std::cout << "\n";
  //starttimer(0, 15);
  for (int i = base; i < nextseqnum; i++){
    sendMessage(i);
  std::cout << "Resending packets.. \n";
  }
  */
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  sentPacket.seqnum = 0;
  sentPacket.acknum = 0;
  windowsize = getwinsize();
  starttimer(0,1);
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  std::cout << "B INPUT: (2) \n";
  bool checked = true;
  int checksum = 0;
  for (int i = 0; i < 20; i++){
      checksum = checksum + int(packet.payload[i]);   //adding up asci values
      std::cout << packet.payload[i];
    }
  checksum = checksum + packet.acknum;
  checksum = checksum + packet.seqnum;
  std::cout << "\n";
  std::cout << "packet.acknum: ";
  std::cout << packet.acknum << "\n";
  std::cout << "packet.seqnum: ";
  std::cout << packet.seqnum << "\n";
  std::cout << "packet.checksum: ";
  std::cout << packet.checksum << "\n";
  std::cout << "Calculated checksum: ";
  std::cout << checksum << "\n"; 
  if (checksum == packet.checksum){
    checked = true;
  } else {
    checked = false;
  }


 /* if (checked) {
    std::cout << "DOUBLE CHECK \n";
  for (int i = 0; i ++; i < prevseqs.size()){
    std::cout << i << "\n";
    if (prevseqs[i] == packet.seqnum){
      std::cout << "flip flag \n";
      checked = false;
    }
  }
  }
  */



  if (checked){

    if ((packet.seqnum >= rcvbase) && (packet.seqnum <= (rcvbase + windowsize-1))){
    std::cout << "Checksum and size match \n";
 
    for (int i = 0; i < 20; i++){
      receivedPacket.payload[i] = packet.payload[i];  //copying data from send to receive for checksum only, and storing it in vector
    }
    
    //mark as ACK'd
    std::cout << "seqnum: ";
    std::cout << packet.seqnum << "\n";
    rcvbufferstatus[packet.seqnum] = 1;
    //updated ack and checksum
    receivedPacket.acknum = packet.seqnum;
    std::cout << "New acknum: ";
    std::cout << receivedPacket.acknum;
    std::cout << "\n";
    receivedPacket.checksum = checksum;
    std::cout << "New checksum: ";
    std::cout << receivedPacket.checksum;
    std::cout << "\n";
    std::cout << "Sending to layer 3A \n"; 
    tolayer3(1, receivedPacket);
    std::cout << "Putting in vector \n"; 
    rcvbuffer[packet.seqnum] = receivedPacket;  
    std::cout << "seqnum: ";
    std::cout << receivedPacket.seqnum << "\n";
    std::cout << "rcvbase: ";
    std::cout << rcvbase << "\n";
  
    if (packet.seqnum == rcvbase){
      int x = rcvbase;
      while (rcvbufferstatus[x] == 1){
        //prevseqs.push_back(packet.seqnum);
        std::cout << "Sending to layer 5B \n";    
        char data[20];
        for (int i = 0; i < 20; i++){
          data[i] = rcvbuffer[x].payload[i];
        }
        tolayer5(1, data);
        //update to uploaded
        rcvbufferstatus[x] = 2;
        x = x +1;
      }
      rcvbase = x;

    }

    


    } else if ((packet.seqnum >= (rcvbase - windowsize)) && (packet.seqnum <= (rcvbase - 1))){
    std::cout << "Previously received \n";
    std::cout << "Generate ACK and send \n";

    for (int i = 0; i < 20; i++){
      receivedPacket.payload[i] = packet.payload[i];  //copying data from send to receive for checksum only
    }
    //updated ack and checksum
    receivedPacket.acknum = packet.seqnum;
    std::cout << "New acknum: ";
    std::cout << receivedPacket.acknum;
    std::cout << "\n";
    receivedPacket.checksum = checksum;
    std::cout << "New checksum: ";
    std::cout << receivedPacket.checksum;
    std::cout << "\n";
    std::cout << "Sending to layer 3A \n";  
    receivedPacket.acknum = -1;
    tolayer3(1, receivedPacket);
  }
  }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  receivedPacket.seqnum = 0;
  receivedPacket.acknum = -1;
}
