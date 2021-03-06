/*
  Modul SD card
  part of Arduino Mega Server project
*/

void SDcardInit() {
  Serial.print("Init SD card... ");
  if (!SD.begin(4)) {
    Serial.println("failed");
    return;
  }
  Serial.println("OK");
  
  if (!SD.exists("index.htm")) {
    Serial.println("Can't find index.htm");
    return;
  }
  Serial.println("index.htm... found");
  modulSdCard = 1;
}


#ifdef SD_FEATURE

void printSDcontent(File dir, int numTabs) {
   while(true) {

     File entry =  dir.openNextFile();
     if (! entry) {
       dir.rewindDirectory();
       break;
     } // no more files
     for (uint8_t i = 0; i < numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printSDcontent(entry, numTabs + 1);
     } else {
       Serial.print("\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void printDirectory(File dir) {
   while(true) {
     File entry = dir.openNextFile();
     if (! entry) {break;} // no more files
     if (! entry.isDirectory()) {
       Serial.println(entry.name());
     } 
     entry.close();
   }
}

String strDirectory(File dir) {
  String str = "";
  dir.rewindDirectory();
  while(true) {
    entry = dir.openNextFile();
    if (! entry) { // no more files
      dir.rewindDirectory();
      return str;
      break;
    }
    if (! entry.isDirectory()) {
      str += entry.name();
      str += " ";
    } 
    entry.close();
  }
}

byte SDcardType() {
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      return 1;
      break;
    case SD_CARD_TYPE_SD2:
      return 2;
      break;
    case SD_CARD_TYPE_SDHC:
      return 3;
      break;
    default:
      return 0;
  }
}

byte SDcardFileSystem() {
  if (!volume.init(card)) {
    return 0;
  } else {
      return volume.fatType();
    }
}

int SDvolumeSize() {
  if (!volume.init(card)) {
    return 0;
  } else {
      uint32_t volumesize;
      volumesize = volume.blocksPerCluster();
      volumesize *= volume.clusterCount();
      volumesize *= 512; // volume size in bytes
      volumesize /= 1024; // volume size in Kbytes
      return volumesize /= 1024; // volume size in Mbytes
    }
}

void cardInfo() {
  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // try to open the partition FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Not find partition");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
 
  volumesize = volume.blocksPerCluster(); // clusters are collections of blocks
  volumesize *= volume.clusterCount(); // we'll have a lot of clusters
  volumesize *= 512; // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFound files:");
  root.openRoot(volume);
 
  // list files in the card
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

#endif // SD_FEATURE
