using System;
using System.Threading.Tasks;
using Windows.Media.Control;
using Windows.Media;
using System.IO.Ports;
using AudioSwitcher.AudioApi.CoreAudio;
using NAudio.CoreAudioApi;

using static System.Runtime.InteropServices.JavaScript.JSType;
using System.Xml.Linq;


public static class Program
{
    //store and compare session info
    private static GlobalSystemMediaTransportControlsSession currentSession;
    //Audio controller for changing each app audio individually
    //private static CoreAudioController audioController = new CoreAudioController();
    //serial port for communication between arduino and PC
    private static SerialPort serialPort;

    private static MMDeviceEnumerator deviceEnumerator = new MMDeviceEnumerator();
    private static MMDevice defaultPlaybackDevice;

    public static string[] VolumeApps = {"chrome","Spotify","Discord","Teams","steamapps"};


    public static async Task Main(string[] args)
    {
        var gsmtcsm = await GetSystemMediaTransportControlsSessionManager();
        // Subscribe to session changes
        gsmtcsm.SessionsChanged += async (sender, e) => await OnSessionsChanged(gsmtcsm);
        currentSession = gsmtcsm.GetCurrentSession();


        defaultPlaybackDevice = deviceEnumerator.GetDefaultAudioEndpoint(DataFlow.Render, Role.Multimedia);
        

        // Initialize the SerialPort
        serialPort = new SerialPort("COM3", 9600); // Replace COM3 with your Arduino's COM port
        serialPort.Parity = Parity.None;
        serialPort.StopBits = StopBits.One;
        serialPort.DataBits = 8;
        serialPort.Handshake = Handshake.None;
        serialPort.RtsEnable = true;
        serialPort.Open();

        //settings serial port data reciving binding to a event so we can work on it.
        serialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

        if (currentSession == null)
        {
            Console.WriteLine("No active media session found.");
        }
        else
        {
            // Subscribe to the media properties changed event
            currentSession.MediaPropertiesChanged += MediaPropertiesChanged;
            await DisplayCurrentMediaProperties();
        }

        Console.WriteLine("Press any key to quit..");
        Console.ReadKey(true);
    }

    private static async Task<GlobalSystemMediaTransportControlsSessionManager> GetSystemMediaTransportControlsSessionManager()
    {
        try
        {
            return await GlobalSystemMediaTransportControlsSessionManager.RequestAsync();
        }
        catch (Exception ex)
        {
            Console.WriteLine("Failed to get media transport controls session manager: " + ex.Message);
            throw;
        }
    }

    private static async Task<GlobalSystemMediaTransportControlsSessionMediaProperties> GetMediaProperties(GlobalSystemMediaTransportControlsSession session)
    {
        try
        {
            return await session.TryGetMediaPropertiesAsync();
        }
        catch (Exception ex)
        {
            Console.WriteLine("Failed to get media properties: " + ex.Message);
            throw;
        }
    }

    private static async void MediaPropertiesChanged(GlobalSystemMediaTransportControlsSession session, MediaPropertiesChangedEventArgs args)
    {
        await DisplayCurrentMediaProperties();
    }

    private static async Task DisplayCurrentMediaProperties()
    {
        var mediaProperties = await GetMediaProperties(currentSession);
        if (mediaProperties != null)
        {
          
            string SerialSend = $"{mediaProperties.Title} - {mediaProperties.Artist}";
            Console.WriteLine(SerialSend);
            
            serialPort.WriteLine(SerialSend);
            

        }
        else
        {
            Console.WriteLine("Failed to retrieve media properties.");
        }
    }

    private static async Task OnSessionsChanged(GlobalSystemMediaTransportControlsSessionManager gsmtcsm)
    {
        var newSession = gsmtcsm.GetCurrentSession();

        if (newSession != null && newSession.SourceAppUserModelId != currentSession?.SourceAppUserModelId)
        {
            if (currentSession != null)
            {
                currentSession.MediaPropertiesChanged -= MediaPropertiesChanged;
            }

            currentSession = newSession;
            currentSession.MediaPropertiesChanged += MediaPropertiesChanged;
            await DisplayCurrentMediaProperties();
        }
    }

    //process the serial data to filter it and in future use for other things then just volume control
    private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
    {
        //Reading the serial data
        string inData = serialPort.ReadLine();

        //Console.WriteLine($"Set {inData}");
        //Actual data filtering for each volume input
        if (inData.Contains("Volume"))
        {
            for (int i = 0; i < VolumeApps.Length; i++)
            {
                string volumeString = inData.Substring(8).Trim();
                if (inData.Contains(i.ToString()))
                {
                    if (int.TryParse(volumeString, out int volume))
                    {
                        Console.WriteLine($"Set {(i.ToString())} volume to {volume}");
                        SetApplicationVolume(VolumeApps[i], volume);
                    }
                }
            }
        }
       /* else if (inData.StartsWith("Volume2:"))
        {
            string volumeString = inData.Substring(8).Trim();
            if (int.TryParse(volumeString, out int volume))
            {
                SetApplicationVolume("Spotify", volume);
            } 
        } */
    }

    //Set application vomume here
    private static void SetApplicationVolume(string appName, int volume)
    {
        // var sessions = audioController.DefaultPlaybackDevice.SessionController.AllSessions();
        var sessions = defaultPlaybackDevice.AudioSessionManager.Sessions;
        
        for (int i = 0; i < sessions.Count; i++)
        {
            var session = sessions[i];
            //Console.WriteLine($"Set {session.ToString}");
            if (session.GetSessionIdentifier.Contains(appName)|| session.DisplayName.Contains(appName))
            {
                session.SimpleAudioVolume.Volume = volume / 100.0f;
                //Console.WriteLine($"Set {appName} volume to {volume}");
            }
        }
    }
}