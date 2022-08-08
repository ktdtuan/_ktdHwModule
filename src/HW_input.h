#ifndef __HW_INPUT_H
#define __HW_INPUT_H

#include <Arduino.h>
#include <Ticker.h>
#include <functional>
#include <FunctionalInterrupt.h>

typedef std::function<void(uint8_t pin, uint8_t level)> fHWInputReport;

class HW_input
{
private:
	uint8_t _pin;
	uint8_t _level;
	fHWInputReport _cb;
	Ticker *tCheck;

	void check(void)
	{
		uint8_t status = digitalRead(_pin);
		if (status != _level && _cb != NULL)
			_cb(_pin, status);
		_level = status;
	}

public:
	/**
	 * @brief Construct a new hw input object
	 *
	 * @param pin GPIO input
	 * @param level level init to push pull
	 * @param cb function callback at GPIO active
	 */
	HW_input(uint8_t pin, uint8_t level, fHWInputReport cb) : _pin(pin), _level(level), _cb(cb)
	{
		pinMode(pin, (level == 0) ? INPUT_PULLUP : INPUT_PULLDOWN);

		this->tCheck = new Ticker;
	}
	~HW_input()
	{
		this->tCheck->detach();
		delete this->tCheck;
	}

	void active(uint32_t time_request_ms)
	{
		this->tCheck->attach_ms<void *>(
			time_request_ms,
			[](void *arg)
			{
				((HW_input *)(arg))->check();
			},
			this);
	}
	void deactive(void)
	{
		this->tCheck->detach();
	}
};

#endif