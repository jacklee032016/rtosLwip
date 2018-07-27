/**
 *
 */

#ifndef CHIP_SYSCLK_H_INCLUDED
#define CHIP_SYSCLK_H_INCLUDED

#include <osc.h>
#include <pll.h>

/**
 * \page sysclk_quickstart Quick Start Guide for the System Clock Management
 * service (SAMV71)
 *
 * This is the quick start guide for the \ref sysclk_group "System Clock
 * Management" service, with step-by-step instructions on how to configure and
 * use the service for specific use cases.
 *
 * \section sysclk_quickstart_usecases System Clock Management use cases
 * - \ref sysclk_quickstart_basic
 * - \ref sysclk_quickstart_use_case_2
 *
 * \section sysclk_quickstart_basic Basic usage of the System Clock Management
 * service
 * This section will present a basic use case for the System Clock Management
 * service. This use case will configure the main system clock to 120MHz,
 * using an internal PLL module to multiply the frequency of a crystal attached
 * to the microcontroller.
 *
 * \subsection sysclk_quickstart_use_case_1_prereq Prerequisites
 *  - None
 *
 * \subsection sysclk_quickstart_use_case_1_setup_steps Initialization code
 * Add to the application initialization code:
 * \code
	sysclk_init();
\endcode
 *
 * \subsection sysclk_quickstart_use_case_1_setup_steps_workflow Workflow
 * -# Configure the system clocks according to the settings in conf_clock.h:
 *    \code sysclk_init(); \endcode
 *
 * \subsection sysclk_quickstart_use_case_1_example_code Example code
 *   Add or uncomment the following in your conf_clock.h header file,
 *   commenting out all other definitions of the same symbol(s):
 *   \code
	   #define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLLACK

	   // Fpll0 = (Fclk * PLL_mul) / PLL_div
	   #define CONFIG_PLL0_SOURCE          PLL_SRC_MAINCK_XTAL
	   #define CONFIG_PLL0_MUL             (120000000UL / BOARD_FREQ_MAINCK_XTAL)
	   #define CONFIG_PLL0_DIV             1

	   // Fbus = Fsys / BUS_div
	   #define CONFIG_SYSCLK_PRES          SYSCLK_PRES_1
\endcode
 *
 * \subsection sysclk_quickstart_use_case_1_example_workflow Workflow
 *  -# Configure the main system clock to use the output of the PLL module as
 *     its source:
 *   \code #define CONFIG_SYSCLK_SOURCE          SYSCLK_SRC_PLLACK \endcode
 *  -# Configure the PLL module to use the fast external fast crystal
 *     oscillator as its source:
 *   \code #define CONFIG_PLL0_SOURCE            PLL_SRC_MAINCK_XTAL \endcode
 *  -# Configure the PLL module to multiply the external fast crystal
 *     oscillator frequency up to 120MHz:
 *   \code
	#define CONFIG_PLL0_MUL             (120000000UL / BOARD_FREQ_MAINCK_XTAL)
	#define CONFIG_PLL0_DIV             1
\endcode
 *   \note For user boards, \c BOARD_FREQ_MAINCK_XTAL should be defined in the
 *   board \c conf_board.h configuration
 *         file as the frequency of the fast crystal attached to the microcontroller.
 *  -# Configure the main clock to run at the full 120MHz, disable scaling of
 *     the main system clock speed:
 *    \code
	#define CONFIG_SYSCLK_PRES         SYSCLK_PRES_1
\endcode
 *    \note Some dividers are powers of two, while others are integer division
 *    factors. Refer to the formulas in the conf_clock.h template commented
 *    above each division define.
 */

/**
 * \page sysclk_quickstart_use_case_2 Advanced use case - Peripheral Bus Clock
 * Management (SAMV71)
 *
 * \section sysclk_quickstart_use_case_2 Advanced use case - Peripheral Bus
 * Clock Management
 * This section will present a more advanced use case for the System Clock
 * Management service. This use case will configure the main system clock to
 * 96MHz, using an internal PLL module to multiply the frequency of a crystal
 * attached to the microcontroller. The USB clock will be configured via the
 * same PLL module.
 *
 * \subsection sysclk_quickstart_use_case_2_prereq Prerequisites
 *  - None
 *
 * \subsection sysclk_quickstart_use_case_2_setup_steps Initialization code
 * Add to the application initialization code:
 * \code
	sysclk_init();
\endcode
 *
 * \subsection sysclk_quickstart_use_case_2_setup_steps_workflow Workflow
 * -# Configure the system clocks according to the settings in conf_clock.h:
 *    \code sysclk_init(); \endcode
 *
 * \subsection sysclk_quickstart_use_case_2_example_code Example code
 *   Add or uncomment the following in your conf_clock.h header file,
 *   commenting out all other definitions of the same symbol(s):
 *   \code
	   #define CONFIG_SYSCLK_SOURCE        SYSCLK_SRC_PLLACK

	   // Fpll0 = (Fclk * PLL_mul) / PLL_div
	   #define CONFIG_PLL0_SOURCE          PLL_SRC_MAINCK_XTAL
	   #define CONFIG_PLL0_MUL             (96000000UL / BOARD_FREQ_MAINCK_XTAL)
	   #define CONFIG_PLL0_DIV             1

	   // Fbus = Fsys / BUS_div
	   #define CONFIG_SYSCLK_PRES          SYSCLK_PRES_1

	   // Fusb = Fsys / USB_div
	   #define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL0
	   #define CONFIG_USBCLK_DIV           2
\endcode
 *
 * \subsection sysclk_quickstart_use_case_2_example_workflow Workflow
 *  -# Configure the main system clock to use the output of the PLL0 module as
 *     its source:
 *   \code #define CONFIG_SYSCLK_SOURCE          SYSCLK_SRC_PLLACK \endcode
 *  -# Configure the PLL0 module to use the fast external fast crystal
 *     oscillator as its source:
 *   \code #define CONFIG_PLL0_SOURCE            PLL_SRC_MAINCK_XTAL \endcode
 *  -# Configure the PLL0 module to multiply the external fast crystal
 *     oscillator frequency up to 96MHz:
 *   \code
	#define CONFIG_PLL0_MUL             (96000000UL / BOARD_FREQ_MAINCK_XTAL)
	#define CONFIG_PLL0_DIV             1
\endcode
 *   \note For user boards, \c BOARD_FREQ_MAINCK_XTAL should be defined in the
 *   board \c conf_board.h configuration file as the frequency of the fast
 *   crystal attached to the microcontroller.
 *  -# Configure the main clock to run at the full 96MHz, disable scaling of
 *     the main system clock speed:
 *    \code
	#define CONFIG_SYSCLK_PRES         SYSCLK_PRES_1
\endcode
 *    \note Some dividers are powers of two, while others are integer division
 *    factors. Refer to the formulas in the conf_clock.h template commented
 *    above each division define.
 *  -# Configure the USB module clock to use the output of the PLL0 module as
 *     its source with division 2:
 *   \code
	#define CONFIG_USBCLK_SOURCE        USBCLK_SRC_PLL0
	#define CONFIG_USBCLK_DIV           2
\endcode
 */


/**
 * \weakgroup sysclk_group
 * @{
 */

//! \name Configuration Symbols
//@{
/**
 * \def CONFIG_SYSCLK_SOURCE
 * \brief Initial/static main system clock source
 *
 * The main system clock will be configured to use this clock during
 * initialization.
 */
#ifndef CONFIG_SYSCLK_SOURCE
# define CONFIG_SYSCLK_SOURCE   SYSCLK_SRC_MAINCK_4M_RC
#endif
/**
 * \def CONFIG_SYSCLK_PRES
 * \brief Initial CPU clock divider (mck)
 *
 * The MCK will run at
 * \f[
 *   f_{MCK} = \frac{f_{sys}}{\mathrm{CONFIG\_SYSCLK\_PRES}}\,\mbox{Hz}
 * \f]
 * after initialization.
 */
#ifndef CONFIG_SYSCLK_PRES
# define CONFIG_SYSCLK_PRES  0
#endif

//@}

//! \name Master Clock Sources (MCK)
//@{
#define SYSCLK_SRC_SLCK_RC         0 //!< Internal 32kHz RC oscillator as master source clock
#define SYSCLK_SRC_SLCK_XTAL       1 //!< External 32kHz crystal oscillator as master source clock
#define SYSCLK_SRC_SLCK_BYPASS     2 //!< External 32kHz bypass oscillator as master source clock
#define SYSCLK_SRC_MAINCK_4M_RC    3 //!< Internal 4MHz RC oscillator as master source clock
#define SYSCLK_SRC_MAINCK_8M_RC    4 //!< Internal 8MHz RC oscillator as master source clock
#define SYSCLK_SRC_MAINCK_12M_RC   5 //!< Internal 12MHz RC oscillator as master source clock
#define SYSCLK_SRC_MAINCK_XTAL     6 //!< External crystal oscillator as master source clock
#define SYSCLK_SRC_MAINCK_BYPASS   7 //!< External bypass oscillator as master source clock
#define SYSCLK_SRC_PLLACK          8 //!< Use PLLACK as master source clock
#define SYSCLK_SRC_UPLLCK          9       //!< Use UPLLCK as master source clock
//@}

//! \name Master Clock Prescalers (MCK)
//@{
#define SYSCLK_PRES_1   PMC_MCKR_PRES_CLK_1  //!< Set master clock prescaler to 1
#define SYSCLK_PRES_2   PMC_MCKR_PRES_CLK_2  //!< Set master clock prescaler to 2
#define SYSCLK_PRES_4   PMC_MCKR_PRES_CLK_4  //!< Set master clock prescaler to 4
#define SYSCLK_PRES_8   PMC_MCKR_PRES_CLK_8  //!< Set master clock prescaler to 8
#define SYSCLK_PRES_16  PMC_MCKR_PRES_CLK_16 //!< Set master clock prescaler to 16
#define SYSCLK_PRES_32  PMC_MCKR_PRES_CLK_32 //!< Set master clock prescaler to 32
#define SYSCLK_PRES_64  PMC_MCKR_PRES_CLK_64 //!< Set master clock prescaler to 64
#define SYSCLK_PRES_3   PMC_MCKR_PRES_CLK_3  //!< Set master clock prescaler to 3
//@}

//! \name Master Clock Division (MCK)
//@{
#define SYSCLK_DIV_1   PMC_MCKR_MDIV_EQ_PCK  //!< Set master clock division to 1
#define SYSCLK_DIV_2   PMC_MCKR_MDIV_PCK_DIV2  //!< Set master clock division to 2
#define SYSCLK_DIV_4   PMC_MCKR_MDIV_PCK_DIV4  //!< Set master clock division to 4
#define SYSCLK_DIV_3   PMC_MCKR_MDIV_PCK_DIV3  //!< Set master clock division to 3
//@}

//! \name USB Clock Sources
//@{
#define USBCLK_SRC_PLL0       0     //!< Use PLLA
#define USBCLK_SRC_UPLL       1     //!< Use UPLL
//@}

/**
 * \def CONFIG_USBCLK_SOURCE
 * \brief Configuration symbol for the USB generic clock source
 *
 * Sets the clock source to use for the USB. The source must also be properly
 * configured.
 *
 * Define this to one of the \c USBCLK_SRC_xxx settings. Leave it undefined if
 * USB is not required.
 */
#ifdef __DOXYGEN__
# define CONFIG_USBCLK_SOURCE
#endif

/**
 * \def CONFIG_USBCLK_DIV
 * \brief Configuration symbol for the USB generic clock divider setting
 *
 * Sets the clock division for the USB generic clock. If a USB clock source is
 * selected with CONFIG_USBCLK_SOURCE, this configuration symbol must also be
 * defined.
 */
#ifdef __DOXYGEN__
# define CONFIG_USBCLK_DIV
#endif

/**
 * \name Querying the system clock
 *
 * The following functions may be used to query the current frequency of
 * the system clock and the CPU and bus clocks derived from it.
 * sysclk_get_main_hz() and sysclk_get_cpu_hz() can be assumed to be
 * available on all platforms, although some platforms may define
 * additional accessors for various chip-internal bus clocks. These are
 * usually not intended to be queried directly by generic code.
 */
//@{

/**
 * \brief Return the current rate in Hz of the main system clock
 *
 * \todo This function assumes that the main clock source never changes
 * once it's been set up, and that PLL0 always runs at the compile-time
 * configured default rate. While this is probably the most common
 * configuration, which we want to support as a special case for
 * performance reasons, we will at some point need to support more
 * dynamic setups as well.
 */
#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
extern uint32_t sysclk_initialized;
#endif
static inline uint32_t sysclk_get_main_hz(void)
{
#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
	if (!sysclk_initialized ) {
		return OSC_MAINCK_4M_RC_HZ;
	}
#endif

	/* Config system clock setting */
	if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_SLCK_RC) {
		return OSC_SLCK_32K_RC_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_SLCK_XTAL) {
		return OSC_SLCK_32K_XTAL_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_SLCK_BYPASS) {
		return OSC_SLCK_32K_BYPASS_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_MAINCK_4M_RC) {
		return OSC_MAINCK_4M_RC_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_MAINCK_8M_RC) {
		return OSC_MAINCK_8M_RC_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_MAINCK_12M_RC) {
		return OSC_MAINCK_12M_RC_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_MAINCK_XTAL) {
		return OSC_MAINCK_XTAL_HZ;
	} else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_MAINCK_BYPASS) {
		return OSC_MAINCK_BYPASS_HZ;
	}
#ifdef CONFIG_PLL0_SOURCE
	else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_PLLACK) {
		return pll_get_default_rate(0);
	}
#endif

#ifdef CONFIG_PLL1_SOURCE
	else if (CONFIG_SYSCLK_SOURCE == SYSCLK_SRC_UPLLCK) {
		return PLL_UPLL_HZ;
	}
#endif
	else {
		/* unhandled_case(CONFIG_SYSCLK_SOURCE); */
		return 0;
	}
}

/**
 * \brief Return the current rate in Hz of the CPU clock
 *
 * \todo This function assumes that the CPU always runs at the system
 * clock frequency. We want to support at least two more scenarios:
 * Fixed CPU/bus clock dividers (config symbols) and dynamic CPU/bus
 * clock dividers (which may change at run time). Ditto for all the bus
 * clocks.
 *
 * \return Frequency of the CPU clock, in Hz.
 */
static inline uint32_t sysclk_get_cpu_hz(void)
{
	/* CONFIG_SYSCLK_PRES is the register value for setting the expected */
	/* prescaler, not an immediate value. */
	return sysclk_get_main_hz() /
		((CONFIG_SYSCLK_PRES == SYSCLK_PRES_3) ? 3 :
			(1 << (CONFIG_SYSCLK_PRES >> PMC_MCKR_PRES_Pos)));
}

/**
 * \brief Retrieves the current rate in Hz of the peripheral clocks.
 *
 * \return Frequency of the peripheral clocks, in Hz.
 */
static inline uint32_t sysclk_get_peripheral_hz(void)
{
	/* CONFIG_SYSCLK_PRES is the register value for setting the expected */
	/* prescaler, not an immediate value. */
	return sysclk_get_main_hz() /
		(((CONFIG_SYSCLK_PRES == SYSCLK_PRES_3) ? 3 : (1 << (CONFIG_SYSCLK_PRES >> PMC_MCKR_PRES_Pos))) * CONFIG_SYSCLK_DIV);
}

/**
 * \brief Retrieves the current rate in Hz of the Peripheral Bus clock attached
 *        to the specified peripheral.
 *
 * \param module Pointer to the module's base address.
 *
 * \return Frequency of the bus attached to the specified peripheral, in Hz.
 */
static inline uint32_t sysclk_get_peripheral_bus_hz(const volatile void *module)
{
	UNUSED(module);
	return sysclk_get_peripheral_hz();
}
//@}

//! \name Enabling and disabling synchronous clocks
//@{

/**
 * \brief Enable a peripheral's clock.
 *
 * \param ul_id Id (number) of the peripheral clock.
 */
static inline void sysclk_enable_peripheral_clock(uint32_t ul_id)
{
	pmc_enable_periph_clk(ul_id);
}

/**
 * \brief Disable a peripheral's clock.
 *
 * \param ul_id Id (number) of the peripheral clock.
 */
static inline void sysclk_disable_peripheral_clock(uint32_t ul_id)
{
	pmc_disable_periph_clk(ul_id);
}

//@}

//! \name System Clock Source and Prescaler configuration
//@{

extern void sysclk_set_prescalers(uint32_t ul_pres);
extern void sysclk_set_source(uint32_t ul_src);

//@}

extern void sysclk_enable_usb(void);
extern void sysclk_disable_usb(void);

extern void sysclk_init(void);

/**
 * \defgroup clk_group Clock Management
 */

/**
 * \ingroup clk_group
 * \defgroup sysclk_group System Clock Management
 *
 * See \ref sysclk_quickstart.
 *
 * The <em>sysclk</em> API covers the <em>system clock</em> and all
 * clocks derived from it. The system clock is a chip-internal clock on
 * which all <em>synchronous clocks</em>, i.e. CPU and bus/peripheral
 * clocks, are based. The system clock is typically generated from one
 * of a variety of sources, which may include crystal and RC oscillators
 * as well as PLLs.  The clocks derived from the system clock are
 * sometimes also known as <em>synchronous clocks</em>, since they
 * always run synchronously with respect to each other, as opposed to
 * <em>generic clocks</em> which may run from different oscillators or
 * PLLs.
 *
 * Most applications should simply call sysclk_init() to initialize
 * everything related to the system clock and its source (oscillator,
 * PLL or DFLL), and leave it at that. More advanced applications, and
 * platform-specific drivers, may require additional services from the
 * clock system, some of which may be platform-specific.
 *
 * \section sysclk_group_platform Platform Dependencies
 *
 * The sysclk API is partially chip- or platform-specific. While all
 * platforms provide mostly the same functionality, there are some
 * variations around how different bus types and clock tree structures
 * are handled.
 *
 * The following functions are available on all platforms with the same
 * parameters and functionality. These functions may be called freely by
 * portable applications, drivers and services:
 *   - sysclk_init()
 *   - sysclk_set_source()
 *   - sysclk_get_main_hz()
 *   - sysclk_get_cpu_hz()
 *   - sysclk_get_peripheral_bus_hz()
 *
 * The following functions are available on all platforms, but there may
 * be variations in the function signature (i.e. parameters) and
 * behavior. These functions are typically called by platform-specific
 * parts of drivers, and applications that aren't intended to be
 * portable:
 *   - sysclk_enable_peripheral_clock()
 *   - sysclk_disable_peripheral_clock()
 *   - sysclk_enable_module()
 *   - sysclk_disable_module()
 *   - sysclk_module_is_enabled()
 *   - sysclk_set_prescalers()
 *
 * All other functions should be considered platform-specific.
 * Enabling/disabling clocks to specific peripherals as well as
 * determining the speed of these clocks should be done by calling
 * functions provided by the driver for that peripheral.
 *
 * @{
 */

//! \name System Clock Initialization
//@{
/**
 * \fn void sysclk_init(void)
 * \brief Initialize the synchronous clock system.
 *
 * This function will initialize the system clock and its source. This
 * includes:
 *   - Mask all synchronous clocks except for any clocks which are
 *     essential for normal operation (for example internal memory
 *     clocks).
 *   - Set up the system clock prescalers as specified by the
 *     application's configuration file.
 *   - Enable the clock source specified by the application's
 *     configuration file (oscillator or PLL) and wait for it to become
 *     stable.
 *   - Set the main system clock source to the clock specified by the
 *     application's configuration file.
 *
 * Since all non-essential peripheral clocks are initially disabled, it
 * is the responsibility of the peripheral driver to re-enable any
 * clocks that are needed for normal operation.
 */

#endif /* CHIP_SYSCLK_H_INCLUDED */
