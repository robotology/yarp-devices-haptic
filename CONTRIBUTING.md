
#### How to add up a new physical haptic device driver

1. **Fork** this repository.
2. Add up the new haptic device within the **drivers directory** following the guidelines you can figure out from the existing drivers.
3. Detil whithin the corresponding section of the `README.md` file the **instructions to install** the new driver on different platforms.
4. Report your name along with your responsabilities within the `README.md` file.
5. Add `add_subdirectory(new_driver)` cmake directive in the `drivers/CMakeLists.txt` file.
6. Carry out **extensive tests**.
7. Make the **pull request**.
