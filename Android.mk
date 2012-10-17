LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= axioctl.c
LOCAL_MODULE := axioctl

LOCAL_STATIC_LIBRARIES := libcutils libc

include $(BUILD_EXECUTABLE)
