extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
}

#include <vector>
#include <array>
#include <string>
#include <sstream>

using Color = std::array<unsigned char, 3>;

bool close(Color a, Color b)
{
    for (int i = 0; i < 3; ++i)
    {
        if (std::abs((int)a[i] - (int)b[i]) > 30)
            return false;
    }

    return true;
}

bool load_image(std::vector<Color> &image,
                const std::string &path, int &x, int &y)
{
    int n;
    unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 0);
    if (data != nullptr)
    {
        for (int i = 0; i < x * y * n; i += n)
        {
            std::array<unsigned char, 3> tmp;
            for (int j = 0; j < 3; ++j)
                tmp[j] = data[i + j];

            image.emplace_back(tmp);
        }
    }
    stbi_image_free(data);
    return (data != nullptr);
}

float find_avg(int w, int h, const std::vector<Color> &imgdata, Color color)
{
    float sum = 0;
    int n = 0;
    for (int x = 0; x < w; ++x)
    {
        for (int y = 0; y < h; ++y)
        {
            if (close(imgdata[y * w + x], color))
            {
                sum += y;
                ++n;
                break;
            }
        }
    }

    return sum / n;
}

float find_avg_interval(const std::vector<float> &averages)
{
    bool increasing = false;
    int last_min = -1;

    int n = 0;
    float sum = 0.f;

    for (size_t i = 1; i < averages.size(); ++i)
    {
        if (averages[i] < averages[i - 1] && !increasing)
        {
            printf("Min at %zu\n", i);
            increasing = true;
            if (last_min == -1)
                last_min = i;
            else
            {
                sum += i - last_min;
                ++n;
            }
        }

        if (averages[i] > averages[i - 1] && increasing)
            increasing = false;
    }

    return sum / n;
}

int main(int argc, char **argv)
{
    int nframes = 0;
    int fps = 60;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-n") == 0)
            nframes = std::stoi(argv[++i]);

        if (strcmp(argv[i], "-fps") == 0)
            fps = std::stoi(argv[++i]);
    }

    if (nframes == 0)
    {
        printf("Number of frames cannot be zero\n");
        exit(EXIT_FAILURE);
    }

    printf("nframes = %d, fps = %d\n", nframes, fps);

    // w and h are always the same
    int w, h;
    std::vector<float> averages;
    for (int i = 0; i < nframes; ++i)
    {
        std::vector<Color> imgdata;
        load_image(imgdata, "frames/" + std::to_string(i) + ".png", w, h);
        averages.emplace_back(find_avg(w, h, imgdata, { 0, 0, 0 }));
    }

    printf("Average frames per minimum: %.2f\n", find_avg_interval(averages));

    return 0;
}
