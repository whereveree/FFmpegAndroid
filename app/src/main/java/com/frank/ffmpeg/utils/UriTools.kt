package com.frank.ffmpeg.utils

import android.content.Context
import android.net.Uri
import android.provider.MediaStore

object UriTools {

    fun isVideo(context: Context, uri: Uri?): Boolean {
        if (uri == null) return false
        if (uri.scheme != "content") return false

        val projection = arrayOf(
            MediaStore.Video.Media._ID,
            MediaStore.Video.Media.MIME_TYPE,
            MediaStore.Video.Media.DURATION
        )

        context.contentResolver.query(uri, projection, null, null, null)?.use {
            if (it.moveToFirst()) {
                val mimeTypeIndex = it.getColumnIndex(MediaStore.Video.Media.MIME_TYPE)
                val mimeType = if (mimeTypeIndex >= 0) it.getString(mimeTypeIndex) else null
                return mimeType?.startsWith("video/") == true
            }
        }
        return false
    }
}