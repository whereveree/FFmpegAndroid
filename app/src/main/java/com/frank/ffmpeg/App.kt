package com.frank.ffmpeg

import android.app.Application
import android.net.Uri

class App : Application() {

    companion object {
        lateinit var app: App
    }

    override fun onCreate() {
        super.onCreate()
        app = this
    }

    

}