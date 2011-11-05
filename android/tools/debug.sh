GDB_CLIENT=/home/javi/android-ndk/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gdb

PID=$(adb shell ps|grep com.games.frikingshark|sed "s/  / /g"|sed "s/  / /g" |sed "s/  / /g"|sed "s/  / /g"|sed "s/  / /g"|cut -d ' ' -f 2)
if [ "$PID" != "" ]; then
	echo Friking shark is running:$PID, killing...
	adb shell kill $PID
fi;

PID=$(adb shell ps|grep gdbserver|sed "s/  / /g"|sed "s/  / /g" |sed "s/  / /g"|sed "s/  / /g"|sed "s/  / /g"|cut -d ' ' -f 2)
if [ "$PID" != "" ]; then
	echo gdbserver is running:$PID, killing...
	adb shell kill $PID
fi;

echo Forwarding ports 
adb forward tcp:5039 tcp:5039
echo Uploading gdbserver 
adb push /home/javi/android-ndk/toolchains/arm-linux-androideabi-4.4.3/prebuilt/gdbserver data/data/com.games.frikingshark/lib
echo Launching activity... 
adb shell am start -a android.intent.action.MAIN -n com.games.frikingshark/android.app.NativeActivity
sleep 1 
# get pid
PID=$(adb shell ps|grep com.games.frikingshark|sed "s/  / /g"|sed "s/  / /g" |sed "s/  / /g"|sed "s/  / /g"|sed "s/  / /g"|cut -d ' ' -f 2)
echo Attaching gdbserver to Friking shark $PID
adb shell data/data/com.games.frikingshark/lib/gdbserver :5039 --attach $PID &
#sleep 1
##echo Starting gdbclient
#$GDB_CLIENT -x "/home/javi/workspace/AndroidPort/android/tools/gdbclientscript.txt"


