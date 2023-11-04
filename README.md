# CSU44053-Computer-Vision
Introductory Lab:OpenCV is an open source computer vision library which was initially developed by Intel. 
It has since taken on a life of its own and is now virtually an industry standard for work in Computer Vision. 
There is a significant body of information on OpenCV available online.
The current release of OpenCV is available at https://opencv.org/releases/. 
The platform used for coursework in this course is the OpenCV libraries under 
Microsoft Visual Studio 2019 and Visual Studio 2019 project files are provided for the sample application. 
The version in use for 2022-23 is v4.6.0 for Windows, and this is part of the standard Windows image on Computer Science machines.
As an example of the use of OpenCV, a complete working application (TIPS - Trinity Image Processing System) is provided for you. 
This application provides working versions of most of the functions taught in the Computer Vision course.
A TIPS zip file (which is referred to as code samples on the linked website) is provided containing the necessary images, 
Visual Studio project file and the full source files for this application. You are asked to:

    1. Download the TIPS zip file  (which is referred to as code samples on the linked website).
        link: https://publications.scss.tcd.ie/book-supplements/A-Practical-Introduction-to-Computer-Vision-with-OpenCV/Code/
    2. Create a "Vision" folder under the C:/ drive (the user data drive) on the machine and uncompress the ZIP file to this location.
    3. Open the Microsoft Visual Studio Solution file: C:/Vision/OpenCVExample/OpenCVExample
    4. Compile the project (preferably in Debug mode).
    5. Execute it and press the number 1, 2, ... , 9 to see the various types of processing.
    6. Now examine the code.  To start with look at main.cpp, Images.cpp, and Histograms.cpp and try to understand what is going on.. 
    7. Make some changes and see what impact they have.
        7.1 Change the images used.
        7.2 Change the parameters used for the various methods..


Assignment1: Table Tennis includes 3 part of code that was built for detecting specific objects in both .jpg and .mkv files

    part I: Locate the table tennis ball.  Using colour, regions and shape locate table tennis balls in the supplied images 
    (See https://www.scss.tcd.ie/Kenneth.Dawson-Howe/Vision/balls.zip for the ball images with ground truth).  
    You can assume that the ball is either white or orange, may have some printing on it and will be spherical.  
    Ensure that you use techniques which can be used in general (e.g. ideally the techniques would cope with changes in lighting, etc.).  
    Analyse how well your approach works on the static images of the tables provided, and later on the table tennis video.  

    part II: Locate the table tennis table.  You must now locate the corners of the table (the outside of the white lines) using edge detection, 
    and then transform the image so that you have a plan view of the table.  Determine how well your approach works on the static images of the tables provided 
    (See https://www.scss.tcd.ie/Kenneth.Dawson-Howe/Vision/tables.zip for the table images with ground truth).  
    Ensure that you use techniques which can be used in general (e.g. ideally the techniques would cope with changes in lighting, etc.).  
    Analyse how well your approach works on the static images of the tables provided, and later on the table tennis video. 

    part III: Process the video of the table tennis game, locating the ball in each frame (if visible).  
    Locate all frames in which the ball changes direction.  Label each of these as 
    (a) ball bounced on the table, (b) ball hit by player, (c) ball hit the net or (d) ball hit something else.  
    Determine your system accuracy on the table tennis video (See https://www.scss.tcd.ie/Kenneth.Dawson-Howe/Vision/TableTennis.avi).  
