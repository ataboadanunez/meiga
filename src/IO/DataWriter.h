#ifndef DataWriter_h_1
#define DataWriter_h_1

#include "Event.h"

class DataWriter {
	
	public:
		DataWriter();
		virtual ~DataWriter();

		static void FileWriter(Event& theEvent);
};

#endif