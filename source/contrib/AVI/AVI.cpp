/**
 @file AVI.cpp
 @maintainer Morgan McGuire, matrix@graphics3d.com
 */

#include "AVI.h"

/** 
 The Windows AVI interface uses global intialization state.  We add a 
 reference count to allow multiple simultaneously open files.  Access this
 through AVIInit and AVIExit.
 */
static void AVIChangeCount(int inc) {
    static int count = 0;
    
    if ((count == 0) && (inc == 1)) {
    	AVIFileInit();
    }

    if ((count == 1) && (inc == -1)) {
        AVIFileExit();
    }

    count += inc;
}

static void AVIInit() {
    AVIChangeCount(1);
}

static void AVIExit() {
    AVIChangeCount(-1);
}

AVIReader::AVIReader(const std::string& filename) : 
    m_ok(false), 
    m_error(""), 
    m_filename(filename), 
    pgf(NULL), 
    nNumAudioStreams(0), 
    nNumVideoStreams(0),
    m_numFrames(0) {
	
    AVIInit();

    if (AVIFileOpen(&pAviFile, filename.c_str(), OF_READ, NULL)) {
        m_error = std::string("Unable to open ") + filename;
    	return;
    }

    // File info
    AVIFileInfo(pAviFile, &info, sizeof(info));

    // Find the audio and video streams
    do {
        if (AVIFileGetStream(pAviFile, &pAudio[nNumAudioStreams],
            streamtypeAUDIO, nNumAudioStreams)) {
            break;
        }
    } while(++nNumAudioStreams < MAX_AUDIO_STREAMS);

    do {
        DWORD result = AVIFileGetStream(pAviFile, &pVideo[nNumVideoStreams], streamtypeVIDEO, nNumVideoStreams);
        if (result != 0) {
            bool noVideo = (result == AVIERR_NODATA);
            (void)noVideo;

            if (result == AVIERR_MEMORY) {
                m_error = std::string("Out of memory while procssing ") + filename;
                return;
            }
            break;
        }
    } while(++nNumVideoStreams < MAX_VIDEO_STREAMS);

    if (nNumVideoStreams == 0) {
        m_error = std::string("No video data found in ") + filename;
        return;
    }

    // Process video stream 0
    // Video stream info (see http://msdn.microsoft.com/library/default.asp?url=/library/en-us/multimed/htm/_win32_avistreaminfo_str.asp)
    AVISTREAMINFO videoInfo;
    if (AVIStreamInfo(pVideo[0], &videoInfo, sizeof(videoInfo))) {
        m_error = std::string("Unable to read video stream 0 info from ") + filename;
        return;
    }

    if (videoInfo.fccType != streamtypeVIDEO) {
        m_error = std::string("Incorrectly labeled video data found in ") + filename;
        return;
    }

    {
        char temp[5];
        temp[0] = (videoInfo.fccHandler >> 24) & 0xFF;
        temp[1] = (videoInfo.fccHandler >> 16) & 0xFF;
        temp[2] = (videoInfo.fccHandler >> 8) & 0xFF;
        temp[3] = videoInfo.fccHandler & 0xFF;
        temp[4] = '\0';
        m_fourCC = std::string(temp);
    }

    m_numFrames = videoInfo.dwLength;

    m_width  = videoInfo.rcFrame.right - videoInfo.rcFrame.left;
    m_height = videoInfo.rcFrame.bottom - videoInfo.rcFrame.top;

    /*
    // Read stream format data
    // Find out how large the format data is (in bytes)
    LONG lSize;
    if (AVIStreamReadFormat(pVideo[0], AVIStreamStart(pVideo[0]), NULL, &lSize)) {
        m_error = std::string("Unable to read stream format on stream 0 from ") + filename;
        return;
    }

    // Read the format data
    LPBYTE pChunk = new BYTE[lSize];
    if (AVIStreamReadFormat(pVideo[0], AVIStreamStart(pVideo[0]), pChunk, &lSize)) {
        m_error = std::string("Unable to read stream format on stream 0 from ") + filename;
        delete[] pChunk;
        pChunk = NULL;
        return;
    }
    delete[] pChunk;
    pChunk = NULL;
    */

    // We always want 24-bit RGB data (this appears to be the most compatible format)

    // (could use the biCompression field to move data around, however that doesn't work on Win98/ME)
    // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/bitmaps_1rw2.asp
    BITMAPINFOHEADER biWanted;
    biWanted.biSize             = sizeof(BITMAPINFOHEADER);
    biWanted.biWidth            = m_width;
    // Normally, negative height means flip upside down (to the format G3D wants),
    // however the AVI functions don't support that convention.
    biWanted.biHeight           = m_height;
    biWanted.biPlanes           = 1;
    biWanted.biBitCount         = 24;
    biWanted.biCompression      = BI_RGB;
    biWanted.biSizeImage        = 0; // May be zero for RGB images
    biWanted.biXPelsPerMeter    = 0;
    biWanted.biYPelsPerMeter    = 0;
    biWanted.biClrUsed          = 3;
    biWanted.biClrImportant     = 0;

    pgf = AVIStreamGetFrameOpen(pVideo[0], &biWanted);
    if (pgf == NULL) {
        m_error = std::string("Unable to read frames from stream 0 as RGB data in ") + filename;
        return;
    }

    m_ok = true;
}


AVIReader::~AVIReader() {

    // Release resources
    if (pgf) {
        if (AVIStreamGetFrameClose(pgf)) {
            // Error
        }
        pgf = NULL;
    }

    for (int i = 0; i < nNumAudioStreams; ++i) {
        AVIStreamRelease(pAudio[i]);
    }

    for (int i = 0; i < nNumVideoStreams; ++i) {
        AVIStreamRelease(pVideo[i]);
    }

    AVIFileRelease(pAviFile);
    pAviFile = NULL;

    AVIExit();
}


void AVIReader::getFrame(int f, GImage& im) {

    LPBITMAPINFOHEADER lpbi;
    lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, f);

    // We know the data is in Windows 32-bit dib format.
    // The 12 constant is a hack--without it this gives textures that 
    // are slightly shifted :(
    const Color3uint8* pData = (Color3uint8*)((uint8*)lpbi + lpbi->biSize + 12);

    im.resize(m_width, m_height, 3);

    // Copy the data from pData to the image, rearranging as necessary
    for (int y = 0; y < m_height; ++y) {
        const Color3uint8* srcRow = pData + y * m_width;
        Color3uint8*       dstRow = im.pixel3() + (m_height - y - 1) * m_width;

        for (int x = 0; x < m_width; ++x) {
            const Color3uint8& src = srcRow[x];
            Color3uint8&       dst = dstRow[x];
            dst.r = src.b;
            dst.g = src.g;
            dst.b = src.r;
        }
    }
}
