package com.frank.ffmpeg.utils

import android.Manifest
import android.os.Build
import com.blankj.utilcode.util.LogUtils
import com.blankj.utilcode.util.PermissionUtils

class PermissionKtTool {

    private val tag = PermissionKtTool::class.java.simpleName

    val PERMISSION_STORAGE = if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.S) {
        arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE)
    } else {
        arrayOf()
    }

    val PERMISSION_CAMERA = arrayOf(Manifest.permission.CAMERA)

    fun checkPermissions(vararg permissions: String): Boolean {
        LogUtils.i(tag, "权限名称:${permissions.toList().toTypedArray().contentToString()}")
        val isGranted = PermissionUtils.isGranted(*permissions)
        LogUtils.i(tag, "是否授权:$isGranted")
        return isGranted
    }

    fun requestPermissions(vararg permissions: String, block: () -> Unit): Boolean {
        return false
    }

    fun checkRequestPermissions() {

    }


}