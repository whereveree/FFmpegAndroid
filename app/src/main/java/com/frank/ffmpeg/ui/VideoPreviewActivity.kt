package com.frank.ffmpeg.ui

import android.media.MediaMetadataRetriever
import android.net.Uri
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.view.SurfaceHolder
import android.view.ViewGroup
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import com.blankj.utilcode.util.LogUtils
import com.blankj.utilcode.util.ScreenUtils
import com.blankj.utilcode.util.ToastUtils
import com.blankj.utilcode.util.UriUtils
import com.frank.androidmedia.controller.MediaPlayController
import com.frank.androidmedia.listener.PlayerCallback
import com.frank.ffmpeg.R
import com.frank.ffmpeg.databinding.ActivityPreviewBinding
import com.frank.ffmpeg.handler.FFmpegHandler.MSG_TOAST
import com.frank.ffmpeg.view.VideoPreviewBar

class VideoPreviewActivity : AppActivity<ActivityPreviewBinding, VideoViewModel>(
    ActivityPreviewBinding::inflate, VideoViewModel::class.java
), VideoPreviewBar.PreviewBarCallback, PlayerCallback {

    private val tag = VideoPreviewActivity::class.java.simpleName
    private lateinit var playController: MediaPlayController
    private lateinit var videoPickerLauncher: ActivityResultLauncher<String>

    private val handler = object : Handler(Looper.getMainLooper()) {
        override fun handleMessage(msg: Message) {
            super.handleMessage(msg)
            if (msg.what == MSG_UPDATE) {
                playController.let {
                    binding.previewVideo.updateProgress(it.currentPosition())
                }
                this.sendEmptyMessageDelayed(MSG_UPDATE, TIME_UPDATE.toLong())
            } else if (msg.what == MSG_TOAST) {
                ToastUtils.showShort(R.string.please_click_select)
            }
        }
    }

    private var filePath = ""

    override fun initUI() {
        binding.surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceCreated(holder: SurfaceHolder) {
                LogUtils.i(tag, "surfaceCreated")
                binding.previewVideo.init(filePath, this@VideoPreviewActivity)
            }

            override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}

            override fun surfaceDestroyed(holder: SurfaceHolder) {}
        })
    }

    override fun initData() {
        super.initData()
        playController = MediaPlayController(this)

        videoPickerLauncher = registerForActivityResult(
            ActivityResultContracts.GetContent()
        ) { uri: Uri? ->
            uri?.let {
                val realPathFromURI = getRealPathFromURI(it)
                filePath = UriUtils.uri2File(it).absolutePath
                LogUtils.i(tag, "realPathFromURI=$realPathFromURI;filePath=$filePath")
                handleVideoUri(uri)
                playController.initPlayer(filePath, binding.surfaceView.holder.surface)
                binding.previewVideo.init(filePath, this)
            } ?: run {}
        }
        videoPickerLauncher.launch("video/*")
    }

    // 可选：获取真实路径的方法（需要处理权限）
    private fun getRealPathFromURI(contentUri: Uri): String? {
        val cursor = contentResolver.query(contentUri, null, null, null, null)
        return cursor?.use {
            it.moveToFirst()
            val columnIndex = it.getColumnIndex("_data")
            if (columnIndex >= 0) it.getString(columnIndex) else null
        }
    }

    private fun handleVideoUri(uri: Uri) {
        val retriever = MediaMetadataRetriever()
        try {
            retriever.setDataSource(this, uri)
            val duration = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)
            val width = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH)?.toInt()
            val height = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT)?.toInt()

            println("视频时长: $duration ms")
            println("视频分辨率: ${width}x$height")
            println("ScreenUtils.getScreenWidth(): ${ScreenUtils.getScreenWidth()}")
            println("ScreenUtils.getScreenWidth(): ${ScreenUtils.getScreenWidth() * width!! / height!!}")

            val layoutParams = binding.surfaceView.layoutParams ?: ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT
            )
            layoutParams.width = ScreenUtils.getScreenWidth()
            layoutParams.height = ScreenUtils.getScreenWidth() * height / width
            binding.surfaceView.layoutParams = layoutParams

        } catch (e: Exception) {
            e.printStackTrace()
        } finally {
            retriever.release()
        }
    }

    override fun onStopTracking(progress: Long) {
        playController.seekTo(progress.toInt())
    }

    override fun onDestroy() {
        super.onDestroy()
        playController.releasePlayer()
        binding.previewVideo.release()
    }

    //region play
    override fun onPrepare() {
        Log.i(TAG, "onPrepare...")
        handler.sendEmptyMessage(MSG_UPDATE)
    }

    override fun onRenderFirstFrame() {
        Log.i(TAG, "onRenderFirstFrame...")
    }

    override fun onError(what: Int, extra: Int): Boolean {
        Log.e(TAG, "onError...")
        return true
    }

    override fun onCompleteListener() {
        Log.i(TAG, "onCompleteListener...")
    }
    //endregion

    companion object {
        private val TAG = VideoPreviewActivity::class.java.simpleName
        private const val TIME_UPDATE = 1000
        private const val MSG_UPDATE = 1234
    }

}
