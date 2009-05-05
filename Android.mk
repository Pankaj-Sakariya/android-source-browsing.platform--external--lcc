#
# Copyright 2009 The Android Open Source Project
#
# Android.mk for building the lcc compiler for arm.
#


BUILD_LCC := false
ifneq ($(TARGET_SIMULATOR),true) # not 64 bit clean

ifeq ($(HOST_OS),darwin)
  BUILD_LCC := true
endif

ifeq ($(HOST_OS),linux)
  BUILD_LCC := true
endif

endif # TARGET_SIMULATOR

ifeq ($(BUILD_LCC),true)

TOP_LOCAL_PATH:= $(call my-dir)

# Build lburg

LOCAL_PATH:= $(TOP_LOCAL_PATH)/lburg
include $(CLEAR_VARS)

LOCAL_MODULE := lcc-lburg

LOCAL_SRC_FILES := \
	gram.c \
	lburg.c

include $(BUILD_HOST_EXECUTABLE)

LLC_LBURG:=$(LOCAL_BUILT_MODULE)

# Build cpp

LOCAL_PATH:= $(TOP_LOCAL_PATH)/cpp
include $(CLEAR_VARS)

LOCAL_MODULE := lcc-cpp

LOCAL_SRC_FILES := \
	cpp.c \
	eval.c \
	getopt.c \
	hideset.c \
	include.c \
	lex.c \
	macro.c \
	nlist.c \
	tokens.c \
	unix.c

include $(BUILD_HOST_EXECUTABLE)

# Build rcc

LOCAL_PATH := $(TOP_LOCAL_PATH)/src
include $(CLEAR_VARS)

LOCAL_MODULE := lcc-rcc

# These two symbols are normally defined by BUILD_XXX, but we need to define them
# here so that local-intermediates-dir works.

LOCAL_IS_HOST_MODULE := true
LOCAL_MODULE_CLASS := EXECUTABLES

intermediates:= $(local-intermediates-dir)/ir
GEN := $(addprefix $(intermediates)/, \
            dagcheck.c \
            alpha.c \
            mips.c \
            sparc.c \
            x86.c \
            x86linux.c \
        )
$(GEN):	PRIVATE_CUSTOM_TOOL = $(LLC_LBURG) $< > $@
$(GEN): $(LLC_LBURG)
$(GEN): $(intermediates)/%.c : $(LOCAL_PATH)/%.md
	$(transform-generated-source)
LOCAL_GENERATED_SOURCES += $(GEN)

LOCAL_SRC_FILES := \
	alloc.c \
	bind.c \
	dag.c \
	decl.c \
	enode.c \
	error.c \
	expr.c \
	event.c \
	init.c \
	inits.c \
	input.c \
	lex.c \
	list.c \
	main.c \
	output.c \
	prof.c \
	profio.c \
	simp.c \
	stmt.c \
	string.c \
	sym.c \
	trace.c \
	tree.c \
	types.c \
	null.c \
	symbolic.c \
	bytecode.c \
	gen.c \
	stab.c \

include $(BUILD_HOST_EXECUTABLE)

# Build lcc

LOCAL_PATH:= $(TOP_LOCAL_PATH)/etc
include $(CLEAR_VARS)

LOCAL_MODULE := lcc

LOCAL_SRC_FILES := \
	lcc.c

ifeq ($(HOST_OS),darwin)
  LOCAL_LDLIBS += -framework Carbon

  LOCAL_SRC_FILES += \
	darwin.c \
	get_my_path_darwin.c
endif

ifeq ($(HOST_OS),linux)

  LOCAL_SRC_FILES += \
	linux.c \
	get_my_path_linux.c
endif


include $(BUILD_HOST_EXECUTABLE)

endif # BUILD_LCC
