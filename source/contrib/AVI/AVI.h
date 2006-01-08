/**
 @file AVI.h
 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite http://www.gamedev.net/reference/articles/article840.asp
 */
#ifndef G3D_AVI_H
#define G3D_AVI_H

#include "graphics3D.h"

#if G3D_VER < 60800
    #error Requires G3D 6.08
#endif

#ifndef _MSC_VER
#   error GVideo is for Win32 only
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "vfw32.lib")

#include <windowsx.h>
#include <mmsystem.h>
#include <vfw.h>

namespace G3D {

/**
 Parses AVI files.  Frames are only read into memory as requested.
 Your machine must have the codec for the file.

 <pre>
    {
        AVIReader avi("C:/Projects/cpp/source/contrib/AVI/test5.avi");
        debugAssertM(avi.ok(), avi.errorString());
        
        GImage im;
        avi.getFrame(0, im);

        texture = Texture::fromGImage(avi.filename(), im);
    }
 </pre>
 */
class AVIReader {
private:

    PAVIFILE        pAviFile;
    
    std::string     m_filename;
    bool            m_ok;
    std::string     m_error;

    enum {MAX_AUDIO_STREAMS = 1,
          MAX_VIDEO_STREAMS = 1};

    PAVISTREAM      pAudio[MAX_AUDIO_STREAMS];
    PAVISTREAM      pVideo[MAX_VIDEO_STREAMS];

    int             nNumAudioStreams;
    int             nNumVideoStreams;

    PGETFRAME       pgf;

    AVIFILEINFO     info;

    int             m_numFrames;

    int             m_width;
    int             m_height;

    /** "fourCC" */
    std::string     m_codec;

public:

    AVIReader(const std::string& filename);

    ~AVIReader();

    const std::string& filename() const {
        return m_filename;
    }

    /** Overwrites any existing contents in im. 
        See G3D::Texture::fromGImage to convert this to a texture
        for rendering.

        @param desiredChannels Must be 1 (grayscale), 3 (RGB), or 4 (RGBA, where A = 255)
     */
    void getFrame(int f, G3D::GImage& im, int desiredChannels = 4);

    bool ok() const {
        return m_ok;
    }

    int numFrames() const {
        return m_numFrames;
    }

    const std::string& errorString() const {
        return m_error;
    }

    /** "fourCC" */
    const std::string& codec() const {
        return m_codec;
    }

    int frameWidth() const {
        return m_width;
    }

    int frameHeight() const {
        return m_height;
    }
};


/**
 Writes AVI files
 @cite http://www.wischik.com/lu/programmer/avi_utils.html
*/
class AVIWriter {
private:
    
    bool            m_ok;
    std::string     m_errorString;
    std::string     m_filename;
    std::string     m_codec;

public:

    /**
     The compression codecs available depend on the drivers installed
     on your machine.

     @param codec A string returned from AVIWriter::getCodecs
     */
    AVIWriter(const std::string& filename, const std::string& codec);

    /** Closes the file */
    ~AVIWriter();

    /** Returns an array of the strings that may be passed to the constructor. */
    static void getCodecs(Array<std::string>& comp);

    bool ok() const {
        return m_ok;
    }

    const std::string& errorString() const {
        return m_errorString;
    }

    /** All frames must have the same dimensions and number of channels. */
    void writeFrame(const GImage& im);

    const std::string& filename() const {
        return m_filename;
    }

    /** "fourCC" */
    const std::string& codec() const {
        return m_codec;
    }
};

}

#endif
