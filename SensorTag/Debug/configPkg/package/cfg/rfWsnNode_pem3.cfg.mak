# invoke SourceDir generated makefile for rfWsnNode.pem3
rfWsnNode.pem3: .libraries,rfWsnNode.pem3
.libraries,rfWsnNode.pem3: package/cfg/rfWsnNode_pem3.xdl
	$(MAKE) -f C:\Users\Tomasz\Desktop\Tomek\Erasmus\Smartwatch\SensorTag/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Tomasz\Desktop\Tomek\Erasmus\Smartwatch\SensorTag/src/makefile.libs clean

