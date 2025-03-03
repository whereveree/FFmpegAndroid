package com.frank.ffmpeg.ui

import android.os.Bundle
import android.os.PersistableBundle
import android.view.KeyEvent
import android.view.LayoutInflater
import android.view.View
import androidx.activity.result.contract.ActivityResultContract
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.viewbinding.ViewBinding
import com.blankj.utilcode.util.ClickUtils
import com.frank.ffmpeg.utils.SINGLE_CLICK_INTERVAL
import com.frank.ffmpeg.extension.ActivityResultLifecycle
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.util.concurrent.atomic.AtomicInteger
import kotlin.coroutines.suspendCoroutine

abstract class AppActivity<VB : ViewBinding, VM : ViewModel>(
    val inflate: (LayoutInflater) -> VB, private val clazz: (Class<VM>),
) : AppCompatActivity() {

    lateinit var binding: VB
    lateinit var viewModel: VM

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = inflate(layoutInflater)
        setContentView(binding.root)
        viewModel = ViewModelProvider(this)[clazz]
        initUI()
        initData()
    }

    protected fun setViewsClickListener(listener: View.OnClickListener, vararg v: View) {
        ClickUtils.applySingleDebouncing(v, SINGLE_CLICK_INTERVAL, listener)
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        when (keyCode) {
            KeyEvent.KEYCODE_BACK -> {
                finish()
                return true
            }
        }
        return super.onKeyDown(keyCode, event)
    }


    open fun initUI() {}

    open fun initData() {}

    private val nextRequestKey = AtomicInteger(0)

    suspend fun <I, O> startActivityForResult(
        contracts: ActivityResultContract<I, O>,
        input: I,
    ): O = withContext(Dispatchers.Main) {
        val requestKey = nextRequestKey.getAndIncrement().toString()

        ActivityResultLifecycle().use { lifecycle, start ->
            suspendCoroutine { c ->
                activityResultRegistry.register(requestKey, lifecycle, contracts) {
                    c.resumeWith(Result.success(it))
                }.apply {
                    start()
                }.launch(input)
            }
        }
    }

}