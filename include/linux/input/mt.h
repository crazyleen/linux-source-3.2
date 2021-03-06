#ifndef _INPUT_MT_H
#define _INPUT_MT_H

/*
 * Input Multitouch Library
 *
 * Copyright (c) 2010 Henrik Rydberg
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/input.h>

#define TRKID_MAX	0xffff

#define INPUT_MT_POINTER	0x0001	/* pointer device, e.g. trackpad */
#define INPUT_MT_DIRECT		0x0002	/* direct device, e.g. touchscreen */
#define INPUT_MT_DROP_UNUSED	0x0004	/* drop contacts not seen in frame */
#define INPUT_MT_TRACK		0x0008	/* use in-kernel tracking */

/**
 * struct input_mt_slot - represents the state of an input MT slot
 * @abs: holds current values of ABS_MT axes for this slot
 */
struct input_mt_slot {
	int abs[ABS_MT_LAST - ABS_MT_FIRST + 1];
};

/**
 * struct input_mt - state of tracked contacts
 * @num_slots: number of MT slots the device uses
 * @flags: input_mt operation flags
 * @frame: increases every time input_mt_sync_frame() is called
 * @slot_frame: last frame at which input_mt_report_slot_state() was called
 * @slots: array of slots holding current values of tracked contacts
 */
struct input_mt {
	int num_slots;
	unsigned int flags;
	unsigned int frame;
	unsigned int *slot_frame;
	int *red;
	struct input_mt_slot slots[];
};

static inline void input_mt_set_value(struct input_mt_slot *slot,
				      unsigned code, int value)
{
	slot->abs[code - ABS_MT_FIRST] = value;
}

static inline int input_mt_get_value(const struct input_mt_slot *slot,
				     unsigned code)
{
	return slot->abs[code - ABS_MT_FIRST];
}

static inline bool input_mt_is_active(const struct input_mt_slot *slot)
{
	return input_mt_get_value(slot, ABS_MT_TRACKING_ID) >= 0;
}

int input_mt_init_slots(struct input_dev *dev, unsigned int num_slots);
int input_mt_init_slots_flags(struct input_dev *dev, unsigned int num_slots,
			unsigned int flags);
void input_mt_destroy_slots(struct input_dev *dev);

static inline int input_mt_new_trkid(struct input_dev *dev)
{
	return dev->trkid++ & TRKID_MAX;
}

static inline void input_mt_slot(struct input_dev *dev, int slot)
{
	input_event(dev, EV_ABS, ABS_MT_SLOT, slot);
}

static inline bool input_is_mt_axis(int axis)
{
	return axis == ABS_MT_SLOT ||
		(axis >= ABS_MT_FIRST && axis <= ABS_MT_LAST);
}

void input_mt_report_slot_state(struct input_dev *dev,
				unsigned int tool_type, bool active);

void input_mt_report_finger_count(struct input_dev *dev, int count);
void input_mt_report_pointer_emulation(struct input_dev *dev, bool use_count);

void input_mt_sync_frame(struct input_dev *dev);

/**
 * struct input_mt_pos - contact position
 * @x: horizontal coordinate
 * @y: vertical coordinate
 */
struct input_mt_pos {
	s16 x, y;
};

int input_mt_assign_slots(struct input_dev *dev, int *slots,
			  const struct input_mt_pos *pos, int num_pos);

#endif
