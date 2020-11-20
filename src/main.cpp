#include "ImageProcess.h"
#include "ImageProcessor.h"
#include "cpr/cpr.h"
#include "imageProcessFunctions.h"
#include "imageUtility.h"
#include "includes.h"
#include "json.hpp"
#include "pureFunctional.h"
#include "screenshot.h"
#include "sleepy_discord/sleepy_discord.h"
#include "sleepy_discord/uwebsockets_websocket.h"
#include "sleepy_discord/websocketpp_websocket.h"

using namespace nlohmann;

// Authorization: Client-ID YOUR_CLIENT_ID
// imgur client-id d96d3c380796c87
// imgur client secret aa524d175bbfc7766048552a98fc5260ac8e2758

const string myId = "158700944425615361";
const string botRequestPrefix = "@farmer";
const string generalChannelId = "267730587308130307";
const string botChannelId = "736094128479141968";
const string channelToUse = botChannelId;

class MyClientClass : public SleepyDiscord::DiscordClient {
  public:
    using SleepyDiscord::DiscordClient::DiscordClient;
    void onMessage(SleepyDiscord::Message message) override {
        if (message.startsWith("@farmer hello")) sendMessage(message.channelID, "HELLO " + message.author.username);
        if (message.startsWith("@farmer tagme")) sendMessage(message.channelID, "Hello <@" + message.author.ID + ">");
        if (message.startsWith("@farmer screen")) {
            cpr::PostCallback(
                [&](cpr::Response r) {
                    cout << "finished" << endl;
                    json j = json::parse(r.text);
                    sendMessage(message.channelID, j["data"]["link"].get<string>());
                },
                cpr::Url{"https://api.imgur.com/3/image"}, cpr::Header{{"Authorization", "Client-ID d96d3c380796c87"}},
                cpr::Multipart{{"image", cpr::File{"./a.bmp"}}});
        }
    }
};

MyClientClass discordClient("NzM2MDU0MjM4Mjg2OTcwOTIx.XxpN7Q.rFakaoM-g0Kw-iDz_kxvSr11CDs", 2);

// int main() {
//     MyClientClass client("NzM2MDU0MjM4Mjg2OTcwOTIx.XxpN7Q.rFakaoM-g0Kw-iDz_kxvSr11CDs", 2);
//     client.run();
// cout << "hello" << endl;

// cpr::Response r =
//     cpr::Post(cpr::Url{"https://api.imgur.com/3/image"}, cpr::Header{{"Authorization", "Client-ID d96d3c380796c87"}},
//               cpr::Multipart{{"image", cpr::File{"./a.bmp"}}});

// cout << r.text << endl;
// }

// window development basics
// https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
// https://docs.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=vs-2019

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);

bool shouldUsePotionOUTDATED(cv::Rect potionArea, float healthBeforeUsingPotion = 0.9) {
    cv::Mat screenShot = takeScreenshot();

    if (screenShot.cols < 1 || screenShot.rows < 1) {
        return false;
    }

    // idea, take red channel and threshold so that anythiung over X value is set to 255, and everything else set to 0
    // find a pixel with a vlue of 255 that is farthest to the right of the health bar region.
    // hp % = x postiion of this pixel / width of region.
    // this works because when ehalth goes down, the red bar goes down and the background doesn't have much red
    // only issue is the friggin whtie text
    // the text in the healthbar is almost white, so it has rgb values pretty high.
    // first let's make it so all pixels with green > certain value are set to 0 that way we get rid of the white text
    // this is because healthbar doesn't have much green

    // get the sub region for the health
    cv::Mat healthRegion = screenShot(potionArea);
    cv::Mat green, blue;

    cv::extractChannel(healthRegion, green, 1);
    cv::extractChannel(healthRegion, blue, 0);

    // find regions that are white and set to black
    cv::Mat whiteRegionMask = green > 100 & blue > 100;

    healthRegion.setTo(cv::Scalar(0, 0, 0), whiteRegionMask);

    // now take the red values and threshold them
    cv::Mat red;
    cv::extractChannel(healthRegion, red, 2);

    red.setTo(0, red < 150);
    red.setTo(255, red >= 150);

    // imshow("mask", whiteRegionMask);
    // imshow("healthRegion", healthRegion);
    // imshow("thresholded", red);

    std::vector<cv::Point2i> locations;
    cv::findNonZero(red, locations);

    auto rightMostPixel = std::max_element(locations.begin(), locations.end(),
                                           [](const cv::Point2i& p1, const cv::Point2i& p2) { return p1.x < p2.x; });

    if (rightMostPixel == locations.end()) {
        return false;
    }
    cout << "health at: " << (*rightMostPixel).x / (float)healthRegion.size().width * 100 << "%" << endl;

    const float potionThreshold = healthBeforeUsingPotion;

    return (*rightMostPixel).x / (float)healthRegion.size().width < potionThreshold;
    // turn all pixels with green values to black

    // imshow("blue", blueChannel);
    // imshow("green", greenChannel);
    // redChannel.reshape(0, )
    return false;
}

bool shouldUsePotion(cv::Rect potionArea) {
    cv::Mat healthRegion = takeScreenshot(potionArea);

    // idea, take red channel and threshold so that anythiung over X value is set to 255, and everything else set to 0
    // find a pixel with a vlue of 255 that is farthest to the right of the health bar region.
    // hp % = x postiion of this pixel / width of region.
    // this works because when ehalth goes down, the red bar goes down and the background doesn't have much red
    // only issue is the friggin whtie text
    // the text in the healthbar is almost white, so it has rgb values pretty high.
    // first let's make it so all pixels with green > certain value are set to 0 that way we get rid of the white text
    // this is because healthbar doesn't have much green

    cv::Mat green, blue;

    cv::extractChannel(healthRegion, green, 1);
    cv::extractChannel(healthRegion, blue, 0);

    // find regions that are white and set to black
    cv::Mat whiteRegionMask = green > 100 & blue > 100;

    healthRegion.setTo(cv::Scalar(0, 0, 0), whiteRegionMask);

    // now take the red values and threshold them
    cv::Mat red;
    cv::extractChannel(healthRegion, red, 2);

    red.setTo(0, red < 150);
    red.setTo(255, red >= 150);

    //

    // imshow("mask", whiteRegionMask);
    // imshow("healthRegion", healthRegion);
    // imshow("thresholded", red);

    std::vector<cv::Point2i> locations;
    cv::findNonZero(red, locations);

    auto rightMostPixel = std::max_element(locations.begin(), locations.end(),
                                           [](const cv::Point2i& p1, const cv::Point2i& p2) { return p1.x < p2.x; });

    if (rightMostPixel == locations.end()) {
        return false;
    }
    cout << "health at: " << (*rightMostPixel).x / (float)healthRegion.size().width << "%" << endl;

    const float potionThreshold = 0.6;

    imshow("potion", healthRegion);
    imshow("threshold", red);

    return (*rightMostPixel).x / (float)healthRegion.size().width < potionThreshold;
    // turn all pixels with green values to black

    // imshow("blue", blueChannel);
    // imshow("green", greenChannel);
    // redChannel.reshape(0, )
    return false;
}

void sendButtonPress(WORD scanCode, bool extendKey = false) {
    INPUT buttonData[2];

    buttonData[0].type = INPUT_KEYBOARD;
    buttonData[0].ki.wVk = 0;
    buttonData[0].ki.dwFlags = 0;
    buttonData[0].ki.dwFlags = KEYEVENTF_SCANCODE;

    if (extendKey) buttonData[0].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;

    buttonData[0].ki.time = 0;
    buttonData[0].ki.wScan = scanCode;

    buttonData[1] = buttonData[0];
    buttonData[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    int sent = SendInput(2, buttonData, sizeof(INPUT));
}

void usePotion() {
    // need both a keyup and a keydown input
    // INPUT potionInput[2];

    // potionInput[0].type = INPUT_KEYBOARD;
    // potionInput[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY;
    // potionInput[0].ki.time = 0;
    // potionInput[0].ki.wScan = 71;

    // potionInput[1] = potionInput[0];
    // potionInput[1].ki.dwFlags |= KEYEVENTF_KEYUP;

    // int sent = SendInput(2, potionInput, sizeof(INPUT));
    sendButtonPress(71, true);  // home

    // cout << "INPUTS SENT:  " << sent << endl;
}

void handleOtherPlayer(const cv::Mat& screenshot = cv::Mat()) {
    sendButtonPress(0x3D);  // f3 key
    sf::sleep(sf::seconds(0.25));
    sendButtonPress(0x3D);

    sf::sleep(sf::seconds(5.5));

    // cash shop button, try to get rid of message box if needed
    sendButtonPress(0x29);  //~
    sendButtonPress(0x29);  //~
    sendButtonPress(0x29);  //~
    sf::sleep(sf::seconds(0.5));
    sendButtonPress(0x29);  //~
}

void stopMacro() {
    sendButtonPress(0x40);  // pause first to prevent buttons from being held down
    sf::sleep(sf::seconds(0.25));
    sendButtonPress(0x3D);  // f3 key
    sendButtonPress(0x3D);
}

void pauseMacro() {
    sendButtonPress(0x40);  // f6 key
}

void resumeMacro() {
    sendButtonPress(0x40);  // f6 key to resume again
}

void attemptChannelChange() {
    sendButtonPress(0x01);  // esc
    sf::sleep(sf::seconds(0.5));

    sendButtonPress(0x1C);  // enter
    sf::sleep(sf::seconds(0.5));

    sendButtonPress(0x4D, true);  // right
    sf::sleep(sf::seconds(0.5));

    // std::terminate();

    sendButtonPress(0x1C);  // enter
    sf::sleep(sf::seconds(0.5));
}

void _sendDiscordMessage(cv::Mat screenShot) {
    cv::Mat output = screenShot;
    // cv::cvtColor(screenShot, output, cv::COLOR_BGR2RGB);
    cv::imwrite("currentScreenshot.bmp", output);

    cpr::PostCallback(
        [&](cpr::Response r) {
            cout << "finished" << endl;
            json j = json::parse(r.text);
            auto imageUrl = j["data"]["link"].get<string>();

            string message = "<@!" + myId + "> \n\nbitch look\n\n" + imageUrl;
            discordClient.sendMessage(channelToUse, message);
        },
        cpr::Url{"https://api.imgur.com/3/image"}, cpr::Header{{"Authorization", "Client-ID d96d3c380796c87"}},
        cpr::Multipart{{"image", cpr::File{"./currentScreenshot.bmp"}}});

    cout << "NO LONGER UPLOADING" << endl;
}

void sendDiscordMessage(cv::Mat& screenShot) {
    std::thread messageThread(_sendDiscordMessage, screenShot);
    messageThread.detach();
}

int main33() {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    cv::Mat playerMapIcon = cv::imread("images/otherplayermapicon.bmp", cv::IMREAD_COLOR);

    cv::Mat imageToSearchIn(std::move(takeScreenshot()));
    cv::Rect regionToSearchIn = cv::selectROI("Select region to search in", imageToSearchIn);

    auto searcher = generateImageSearch(playerMapIcon, regionToSearchIn)(imageToSearchIn);
    cv::waitKey(0);
}

int main() {
    std::thread discordThread([&]() { discordClient.run(); });
    try {
        // initialize gdiplus
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        sf::RenderWindow window(sf::VideoMode(800, 600), "Gooddaysir");

        sf::Event event;

        cv::Rect potionArea;
        cv::Rect mapArea;

        const sf::Clock timer;
        sf::Time lastPotionCheckTime = sf::milliseconds(0);
        sf::Time potionDelay = sf::seconds(0.5);

        bool areRegionsSelected = false;
        bool started = false;

        cv::Mat playerMapIcon = cv::imread("images/otherplayermapicon.bmp", cv::IMREAD_COLOR);
        auto otherPlayerOnMap = function<bool(const cv::Mat&)>(generateImageSearch(playerMapIcon, mapArea));
        float minHealth = 0.5;

        // cout << "enter the amount you want to suck in health" << endl;
        // cin >> minHealth;

        // ImageProcessor processor(sf::seconds(0.5));
        // processor.every(1, "avoid player", generatePlayerAvoidanceFunction(mapArea, playerMapIcon, handleOtherPlayer));

        // shared_ptr<ImageProcess> playerAvoidancedProcess = makeshared<PlayerAvoidancePRocess>(region);
        // processor.addProcess("player avoidance", 1, playerAvoidancedProcess);

        // playerAvoidanceProcess->setActiveImageRegion(region);
        // shared_ptr<ImageProcess> playerAvoidanceProcess = makeShared<ImageProcess>(region, _when(otherPlayerOnMap,
        // handleOtherPlayer)); processor.addPRocess("avoid player", 1, playerAvoidancedProcess);

        // playerAvoidanceProcess->updateRegion()
        // RegionChecker

        // imageProcessor.every(1, "avoid player", _when(otherPlayerOnMap(mapRegion), avoidPlayer));
        // imageProcessor.updateProcess("avoid player", _when(otherPlayerOnMap, avoidPlayer));

        /*

        auto otherPlayerOnMap = generateImageSearcher(playerMapIcon, mapRegion);

        auto otherPlayerOnMap = [](cv::Rect region, cv::Mat& image) {
            cv::Mat mapOnly = image(region);

            return imageContains(playerMapIcon, image);
        };

        auto avoidPlayers = [&]() {
            // stop macro
            sendButtonPress(f3);

            // wait for some time before doing something else, that way player isn't in attacking state
            // takes 5 seconds for paleyr to catch his breath
            // be careful so that anti afk mobs don't kill you lol
            sf::sleep(sf::seconds(5.5));

            // go to cashshop, easy cuz unlike CC it happens instantly
            openCashShop();
        };

        imageProcessor.every(2 cycles, "avoid players", _when(otherPlayerOnMap, avoidPlayers))
                .every(1 cycle, "use potion", )

                    cout
            << "press S to configure healthbar location" << endl;*/

        cout << "Press S to configure Healthbar and map location" << endl;
        cout << "Press O to start image bot, PRESS P TO STOP" << endl;

        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    cv::Mat image(std::move(takeScreenshot()));
                    potionArea = cv::selectROI("Select Hp Region", image);
                    mapArea = cv::selectROI("Select Map Region", image);
                    // areRegionsSelected = potionArea.area() > 0;

                    areRegionsSelected = potionArea.area() > 0 && mapArea.area() > 0;

                    if (areRegionsSelected) {
                        otherPlayerOnMap = generateImageSearch(playerMapIcon, mapArea);
                        // processor.every(1, "avoid players", otherPlayerOnMap);
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                    started = false;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) {
                    cv::Mat image(std::move(takeScreenshot()));
                    sendDiscordMessage(image);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O) {
                    started = true && areRegionsSelected;
                    if (!started) cout << "REGIONS NOT SET UP YET YOU DUMB FUCK" << endl;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                    cv::Mat image(std::move(takeScreenshot()));
                    mapArea = cv::selectROI("Select Map Region", image);
                    otherPlayerOnMap = generateImageSearch(playerMapIcon, mapArea);
                    otherPlayerOnMap(image);
                }
            }

            if (areRegionsSelected && timer.getElapsedTime() - lastPotionCheckTime > potionDelay && started) {
                lastPotionCheckTime = timer.getElapsedTime();
                // cout << "CHECKING" << endl;
                if (shouldUsePotionOUTDATED(potionArea, minHealth)) {
                    usePotion();
                }

                cv::Mat screen(std::move(takeScreenshot()));
                if (otherPlayerOnMap(screen)) {
                    cout << "PLAYER ON MAP" << endl;
                    cv::imshow("BIG PICTURE PLAYER FOUND", screen);
                    sendDiscordMessage(screen);
                    // handleOtherPlayer();
                    pauseMacro();
                    sf::sleep(sf::seconds(5.5));  // sleep to catch breath
                    usePotion();                  // use potion incase you about to die

                    // try to keep changing channels till players are gone
                    do {
                        // always attempt to cc twice in quick succession
                        attemptChannelChange();
                        // attemptChannelChange();
                        sf::sleep(sf::seconds(4.5));  // wait incase it takes forever to load next channel
                    } while (otherPlayerOnMap(std::move(takeScreenshot())));

                    usePotion();
                    resumeMacro();
                    // started = false;
                }
            }

            window.clear();
            window.display();
        }

        GdiplusShutdown(gdiplusToken);
    } catch (std::exception& e) {
        cout << "ERROR CRASH" << endl;
        cout << e.what() << endl;
        int x;
        cin >> x;
    }
    discordClient.quit();
    discordThread.join();
}

int main5() {
    try {
        // initialize gdiplus
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        sf::RenderWindow window(sf::VideoMode(800, 600), "Gooddaysir");

        sf::Event event;

        const sf::IntRect rect(887, 1028, 170, 13);
        cv::Rect potionArea;

        const sf::Clock timer;
        sf::Time lastPotionCheckTime = sf::milliseconds(0);
        sf::Time potionDelay = sf::seconds(2);

        bool areRegionsSelected = false;

        cout << "press S to configure healthbar location" << endl;

        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    cv::Mat image(std::move(takeScreenshot()));
                    cv::namedWindow("test");
                    potionArea = cv::selectROI("test", image);

                    areRegionsSelected = true;
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                    shouldUsePotionOUTDATED(potionArea);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) {
                    for (unsigned i = 0; i < 1000; ++i) {
                        sf::Clock smol;
                        // if (shouldUsePotionOUTDATED(potionArea)) {
                        //     usePotion();
                        // }
                        takeScreenshot();
                        cout << smol.getElapsedTime().asSeconds() << endl;
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::O) {
                    usePotion();
                }
            }

            if (areRegionsSelected && timer.getElapsedTime() - lastPotionCheckTime > potionDelay) {
                lastPotionCheckTime = timer.getElapsedTime();
                sf::Clock smol;
                if (shouldUsePotionOUTDATED(potionArea)) {
                    usePotion();
                }
                cout << smol.getElapsedTime().asSeconds() << endl;
            }

            window.clear();
            window.display();
        }

        GdiplusShutdown(gdiplusToken);
    } catch (std::exception& e) {
        cout << "ERROR CRASH" << endl;
        cout << e.what() << endl;
        int x;
        cin >> x;
    }
}

// int howIWantImagingToWork() {
//     mapRegion(fromCv);

//     imageProcessor.update();

//     imageProcesser
//         .addProcess("autoPotion", when(
//             and(
//                 regionSatisfies(runeHasSpawned, mapRegion), regionSatisfies(runeCooldownNotInEffect, buffRegion)
//             ),
//             beginRuneHunting
//             )
//         healthRegion.when(healthIsLow), 1 second, priority 1
//         // When(healthWatcher.isLowOnHealth, usePotion), 1 second, priority 1)

//             imageProcessor
//         .addProcess("playerAvoidance", When(mapWatcher.otherPlayerOnMap, macroBot.changeChannels), 1 second, priority 2, )

//             imageProcessor
//         .addProcess("runeWatcher",
//                     When(and(mapWatcher.runeHasSpawned, buffWatcher.runeCooldownNotInEffect), beginRuneHunting), 10
//                     seconds, priority 3)

//             imageProcessor
//         .addProcess("potionLockResponder", When(or (antiBotEnemyHasSpawned, potionLockInEffect), useInstantKill), 1
//         second,
//                     priority 0)

//             imagePRocessor.addProcess("escapeInstantDeathEliteMobSkill",
//                                       When(trappedInInstantKillHold, escapeInstantKillHold), 2 second, priority 0)

//                 When there is another player in the map,
//         stop macro, and return to town ? or CC

//                                                  When health region watcher says that the player is low on health,
//         use a potion assigned to a hotkey
// }

int main2() {
    // initialize gdiplus
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // initialize to 0 or whatever
    WNDCLASSA windClass = {};

    // ignore intellisense error about LPCstr or whatever, compiles fine
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassa
    windClass.lpszClassName = TEXT("testg");
    windClass.lpfnWndProc = WndProc;
    windClass.hInstance = GetModuleHandleA(NULL);

    RegisterClass(&windClass);

    // create message only window
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
    // ignore intellisense error about the classname cuz it compiles fine
    HWND hWnd = CreateWindowEx(0, windClass.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);

    if (!hWnd) {
        cout << "error creating window" << endl;
        cout << GetLastError() << endl;
        return -1;
    }

    // register raw input
    // https://docs.microsoft.com/en-us/windows/win32/inputdev/using-raw-input
    RAWINPUTDEVICE keyboard;
    keyboard.usUsagePage = 0x0001;
    keyboard.usUsage = 0x0006;
    keyboard.hwndTarget = hWnd;
    keyboard.dwFlags = RIDEV_EXINPUTSINK;

    if (RegisterRawInputDevices(&keyboard, 1, sizeof(keyboard)) == FALSE) {
        cout << "Faield to register raw input device" << endl;
        return 0;
    }
    int numScreenshots = 1000;
    cout << "Enter number of screenshots to take in order to compute the average time and press enter" << endl;
    cin >> numScreenshots;
    cout << "taking " << numScreenshots << " screenshots" << endl;
    sf::Time totalTime = sf::milliseconds(0);
    sf::Clock overallTimer;
    for (unsigned i = 0; i < numScreenshots; i++) {
        sf::Clock timer;
        takeScreenshot();
        totalTime += timer.getElapsedTime();
    }
    float num = overallTimer.getElapsedTime().asSeconds();
    cout << "Total time for all " << numScreenshots << "  screenshot(s):  " << num << "  seconds" << endl;
    cout << "Average screenshot time: " << totalTime.asSeconds() / numScreenshots << "  seconds" << endl;
    cout << "Could do a recording at: " << 1 / (totalTime.asSeconds() / numScreenshots) << " FPS" << endl;
    char x;
    cout << "type in any key and hit enter to finish" << endl;
    cin >> x;
    return 0;

    // message loop
    // https://docs.microsoft.com/en-us/windows/win32/learnwin32/window-messages
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    cout << "successfully created everyting" << endl;
    return 0;
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    switch (message) {
        // raw inptu since that's what we registered for
        // https://docs.microsoft.com/en-us/windows/win32/inputdev/using-raw-input
        case WM_INPUT: {
            // first get size of the rawinput data
            UINT bufferSize;

            // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getrawinputdata
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

            // have the size, now we create buffer
            LPBYTE buffer = new BYTE[bufferSize];

            // load data into buffer
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &bufferSize, sizeof(RAWINPUTHEADER));

            RAWINPUT* input = (RAWINPUT*)buffer;

            // https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawkeyboard
            if (input->header.dwType == RIM_TYPEKEYBOARD) {
                auto keyboard = input->data.keyboard;
                bool isDown = keyboard.Flags & RI_KEY_BREAK;
                bool e0 = keyboard.Flags & RI_KEY_E0;
                bool e1 = keyboard.Flags & RI_KEY_E1;

                cout << "make: " << (char)MapVirtualKeyA(input->data.keyboard.VKey, MAPVK_VK_TO_CHAR) << " / "
                     << input->data.keyboard.VKey << ", is " << (!isDown ? "pressed down" : "released")
                     << " . scan code: " << keyboard.MakeCode << " / "
                     << MapVirtualKeyA(input->data.keyboard.MakeCode, MAPVK_VSC_TO_VK) << " , e0: " << e0 << ", e1: " << e1
                     << "; FLAGS: " << keyboard.Flags << endl;
            }

            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}