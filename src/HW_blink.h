#ifndef __HW_BLINK_H_
#define __HW_BLINK_H_

#include <Arduino.h>
#include <Ticker.h>
#include <functional>

typedef std::function<void(uint8_t pin, uint8_t level)> fHWOutputReport;

class HW_blink
{
private:
	uint8_t _pin;
	uint8_t _level;
	fHWOutputReport _cb;
	Ticker *interval;

	struct
	{
		int32_t timeOn;
		int32_t timeOff;
		uint32_t retry;
	} config;

	void blinkOn(void)
	{
		this->interval->once_ms<void *>(
			config.timeOn,
			[](void *arg)
			{
				((HW_blink *)(arg))->blinkOff();
			},
			this);

		this->on();

		// report
		if (this->_cb != NULL)
			this->_cb(this->_pin, !this->_level);
	}

	void blinkOff(void)
	{
		if (--config.retry > 0)
		{
			this->interval->once_ms<void *>(
				config.timeOff,
				[](void *arg)
				{
					((HW_blink *)(arg))->blinkOn();
				},
				this);
		}

		this->off();

		//  report
		if (this->_cb != NULL)
			this->_cb(this->_pin, this->_level);
	}

public:
	/**
	 * @brief Construct a new hw blink object
	 *
	 * @param pin GPIO blink
	 * @param level level nomal start
	 * @param cb function callback toggle active
	 */
	HW_blink(uint8_t pin, uint8_t level, fHWOutputReport cb) : _pin(pin), _level(level), _cb(cb)
	{
		this->interval = new Ticker;

		pinMode(pin, OUTPUT);
		digitalWrite(pin, level);
	}
	~HW_blink()
	{
		this->interval->detach();
		delete this->interval;
	}

	void blink(int32_t timeOn, int32_t timeOff, uint32_t retry)
	{
		this->config.timeOn = timeOn;
		this->config.timeOff = timeOff;
		this->config.retry = retry;

		this->blinkOn();
	}

	void on(void)
	{
		digitalWrite(this->_pin, !this->_level);
	}
	void off(void)
	{
		digitalWrite(this->_pin, this->_level);
	}
};

// this->interval->once_ms<config_t *>(
// 	_config->timeOff,
// 	[](config_t *arg)
// 	{ ((HW_blink *)(arg))->blink(arg); },
// 	&this->config);
#endif
