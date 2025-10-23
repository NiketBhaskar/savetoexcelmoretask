import QtQuick 2.9
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtCharts 2.13

Item { //Window//if button
    id :root
    width: 300
    height: 300
    property int xVal : 0;
    property double yVal : 2;

    function setAttenDVal(preciseRegion, estimatedRegion, filteredRegion, glanceTimeVal, glanceCountVal, liveAttenDVal, meanAttenDVal, driverState, blinkVal)
    {
       // yVal = meanAttenDVal;
        yVal = liveAttenDVal;
        //console.log("yval",yVal);
    }

    Timer{
        id : timeInc
        repeat  : true
        running : true
        interval : 1000
        onTriggered :
        {
            //if(xVal==0)  series1.append(0, 2);

            series1.append(xVal, yVal);
            xVal+=1;
            //series2.append(xVal, yVal);
            //console.log(" atten ",xVal,yVal)
        }
    }

    //GROUP BOX FOR EYE TRACKER/////////////////////////////////////////////
    GroupBox {
        id: meanAttenDValueGraphBox
        x: 5
        y: 10
        width: 300
        height:300
        title: "  AttenD Graph"
        z: -1
        ChartView{
        title  :"AttenD Mean Graph"
           anchors.fill : parent
           antialiasing : true

           ValueAxis {
               id : xAxis
               min : 0
               max :xVal
           }

           ValueAxis  {
               id :yAxis
               min : 0
               max: 2
           }

           LineSeries  {
               id:series1
               name  :"Mean Value"
               axisX:xAxis
               axisY:yAxis
            //   XYPoint { x 0; y 0 }
            //   XYPoint { x xVal; y yVal }
           }
           Button{
               id:clearbtn
               x:230
               y:20
               text: "clc"
               height:30
               width:30
               onClicked: {
                   series1.clear();
                   yVal=2;
                   xVal=0;
               }
           }

         }

    }

}
