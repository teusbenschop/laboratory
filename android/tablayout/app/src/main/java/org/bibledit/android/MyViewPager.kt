package org.bibledit.android

import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import androidx.viewpager.widget.ViewPager


class MyViewPager(context: Context, attrs: AttributeSet) : ViewPager(context, attrs) {

    var pagingEnabled: Boolean = false

//    override fun onTouchEvent(event: MotionEvent): Boolean {
//        println("onTouchEvent")
//        return if (this.pagingEnabled) {
//            super.onTouchEvent(event)
//        } else false
//    }

    override fun onInterceptTouchEvent(event: MotionEvent): Boolean {
        println("onInterceptTouchEvent")
        return if (this.pagingEnabled) {
            super.onInterceptTouchEvent(event)
        } else false
    }
}
