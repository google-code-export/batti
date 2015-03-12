# Frequently Asked Questions #

---


**How can I assign my own sound for a sound event?**

You have to put your sound file to `\windows` folder. Then start **Batti Configuration** and assign the sound for the event on **Advanced** page.

**My device displays bad values of temperature and/or voltage. What can I do with it?**

The problem is, that not all manufacturers follow the MS specification on how to report values correctly. Batti is reading all values via standard API provided by OS.

To correct the problem, you can set correction coefficients for:

  * temperature (cT),
  * voltage of main battery (cV),
  * voltage of backup battery (cBV).

Just create a registry value (String) in `HKCU\Software\DaProfik\Batti` named according to the text in paretheses in above list (cT means correction coeficient for temperature etc.). If you need 10 times larger value, set its value to 10.0.

Example:

  1. Batti reports **2.8 C** as battery temperature.
  1. Create registry value named **cT** (typed as String) under `HKCU\Software\DaProfik\Batti`.
  1. Set its value to **10.0**.
  1. Tap the Batti's bar to see the corrected value (28 C in our example).
  1. Restart Batti.