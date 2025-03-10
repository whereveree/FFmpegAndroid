
#include "lame.h"
#include <jni.h>

#define MP3_FUNC(RETURN_TYPE, FUNC_NAME, ...) \
    JNIEXPORT RETURN_TYPE JNICALL Java_com_frank_mp3_Mp3Lite_ ## FUNC_NAME \
    (JNIEnv *env, jclass thiz, ##__VA_ARGS__)\

lame_global_flags *global_flags;

lame_global_flags *initialize(
        JNIEnv *env,
        jint inSampleRate, jint outChannel,
        jint outSampleRate, jint outBitrate, jfloat scaleInput, jint mode, jint vbrMode,
        jint quality, jint vbrQuality, jint abrMeanBitrate, jint lowpassFreq, jint highpassFreq,
        jstring id3tagTitle, jstring id3tagArtist, jstring id3tagAlbum,
        jstring id3tagYear, jstring id3tagComment) {

    lame_global_flags *glf = lame_init();
    lame_set_in_samplerate(glf, inSampleRate);
    lame_set_num_channels(glf, outChannel);
    lame_set_out_samplerate(glf, outSampleRate);
    lame_set_brate(glf, outBitrate);
    lame_set_quality(glf, quality);
    lame_set_scale(glf, scaleInput);
    lame_set_VBR_q(glf, vbrQuality);
    lame_set_VBR_mean_bitrate_kbps(glf, abrMeanBitrate);
    lame_set_lowpassfreq(glf, lowpassFreq);
    lame_set_highpassfreq(glf, highpassFreq);

    switch (mode) {
        case 0:
            lame_set_mode(glf, STEREO);
            break;
        case 1:
            lame_set_mode(glf, JOINT_STEREO);
            break;
        case 3:
            lame_set_mode(glf, MONO);
            break;
        case 4:
        default:
            lame_set_mode(glf, NOT_SET);
            break;
    }

    switch (vbrMode) {
        case 0:
            lame_set_VBR(glf, vbr_off);
            break;
        case 2:
            lame_set_VBR(glf, vbr_rh);
            break;
        case 3:
            lame_set_VBR(glf, vbr_abr);
            break;
        case 4:
            lame_set_VBR(glf, vbr_mtrh);
            break;
        case 6:
            lame_set_VBR(glf, vbr_default);
            break;
        default:
            lame_set_VBR(glf, vbr_off);
            break;

    }

    const jchar *title = NULL;
    const jchar *artist = NULL;
    const jchar *album = NULL;
    const jchar *year = NULL;
    const jchar *comment = NULL;
    if (id3tagTitle) {
        title = (*env)->GetStringChars(env, id3tagTitle, NULL);
    }
    if (id3tagArtist) {
        artist = (*env)->GetStringChars(env, id3tagArtist, NULL);
    }
    if (id3tagAlbum) {
        album = (*env)->GetStringChars(env, id3tagAlbum, NULL);
    }
    if (id3tagYear) {
        year = (*env)->GetStringChars(env, id3tagYear, NULL);
    }
    if (id3tagComment) {
        comment = (*env)->GetStringChars(env, id3tagComment, NULL);
    }

    if (title || artist || album || year || comment) {
        id3tag_init(glf);

        if (title) {
            id3tag_set_title(glf, (const char *) title);
            (*env)->ReleaseStringChars(env, id3tagTitle, title);
        }
        if (artist) {
            id3tag_set_artist(glf, (const char *) artist);
            (*env)->ReleaseStringChars(env, id3tagArtist, artist);
        }
        if (album) {
            id3tag_set_album(glf, (const char *) album);
            (*env)->ReleaseStringChars(env, id3tagAlbum, album);
        }
        if (year) {
            id3tag_set_year(glf, (const char *) year);
            (*env)->ReleaseStringChars(env, id3tagYear, year);
        }
        if (comment) {
            id3tag_set_comment(glf, (const char *) comment);
            (*env)->ReleaseStringChars(env, id3tagComment, comment);
        }
    }

    lame_init_params(glf);


    return glf;
}

jint encode(JNIEnv *env, lame_global_flags *glf,jshortArray buffer_l, jshortArray buffer_r,jint samples, jbyteArray mp3buf) {
    jshort *j_buffer_l = (*env)->GetShortArrayElements(env, buffer_l, NULL);
    jshort *j_buffer_r = (*env)->GetShortArrayElements(env, buffer_r, NULL);

    const jsize mp3buf_size = (*env)->GetArrayLength(env, mp3buf);
    jbyte *j_mp3buf = (*env)->GetByteArrayElements(env, mp3buf, NULL);

    int result = lame_encode_buffer(glf, j_buffer_l, j_buffer_r,samples, j_mp3buf, mp3buf_size);

    (*env)->ReleaseShortArrayElements(env, buffer_l, j_buffer_l, 0);
    (*env)->ReleaseShortArrayElements(env, buffer_r, j_buffer_r, 0);
    (*env)->ReleaseByteArrayElements(env, mp3buf, j_mp3buf, 0);

    return result;
}

jint encodeBufferInterleaved(JNIEnv *env, lame_global_flags *glf, jshortArray pcm, jint samples, jbyteArray mp3buf) {
    jshort *j_pcm = (*env)->GetShortArrayElements(env, pcm, NULL);

    const jsize mp3buf_size = (*env)->GetArrayLength(env, mp3buf);
    jbyte *j_mp3buf = (*env)->GetByteArrayElements(env, mp3buf, NULL);

    int result = lame_encode_buffer_interleaved(glf, j_pcm,samples, j_mp3buf, mp3buf_size);

    (*env)->ReleaseShortArrayElements(env, pcm, j_pcm, 0);
    (*env)->ReleaseByteArrayElements(env, mp3buf, j_mp3buf, 0);

    return result;
}

jint flush(JNIEnv *env, lame_global_flags *glf, jbyteArray mp3buf) {
    const jsize mp3buf_size = (*env)->GetArrayLength(env, mp3buf);
    jbyte *j_mp3buf = (*env)->GetByteArrayElements(env, mp3buf, NULL);

    int result = lame_encode_flush(glf, j_mp3buf, mp3buf_size);

    (*env)->ReleaseByteArrayElements(env, mp3buf, j_mp3buf, 0);

    return result;
}

void close_lame(lame_global_flags *glf) {
    lame_close(glf);
    glf = NULL;
}


MP3_FUNC(void, lameInitDefault) {
    global_flags = lame_init();
    lame_init_params(global_flags);
}

MP3_FUNC(void, lameInit,
         jint inSampleRate, jint outChannel, jint outSampleRate, jint outBitrate,
         jfloat scaleInput, jint mode, jint vbrMode, jint quality, jint vbrQuality,
         jint abrMeanBitrate, jint lowPassFreq, jint highPassFreq, jstring id3tagTitle,
         jstring id3tagArtist, jstring id3tagAlbum, jstring id3tagYear,
         jstring id3tagComment) {

    global_flags = initialize(env, inSampleRate, outChannel, outSampleRate, outBitrate, scaleInput, mode,
                              vbrMode,
                              quality, vbrQuality, abrMeanBitrate, lowPassFreq, highPassFreq, id3tagTitle,
                              id3tagArtist, id3tagAlbum,
                              id3tagYear,
                              id3tagComment);
}

MP3_FUNC(jint, lameEncode, jshortArray buffer_l, jshortArray buffer_r, jint samples, jbyteArray mp3buf) {
    return encode(env, global_flags, buffer_l, buffer_r, samples, mp3buf);
}

MP3_FUNC(jint, encodeBufferInterleaved, jshortArray pcm, jint samples, jbyteArray mp3buf) {
    return encodeBufferInterleaved(env, global_flags, pcm, samples, mp3buf);
}

MP3_FUNC(jint, lameFlush, jbyteArray mp3buf) {
    return flush(env, global_flags, mp3buf);
}

MP3_FUNC(void, lameClose) {
    close_lame(global_flags);
}