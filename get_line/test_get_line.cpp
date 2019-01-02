#include <cstdlib>
#include "gtest/gtest.h"

#include "get_line.h"

TEST(TestReadLine, AutoBufferAllocationFileRead)
{
	static char file_memory_buffer[] = "This is a test\nfile with 2 lines.";

	FILE *const fh = fmemopen(file_memory_buffer, sizeof(file_memory_buffer), "r");
	ASSERT_NE(fh, nullptr);

	char *buffer = nullptr;
	size_t buffer_size;
	int result = read_line(fh, &buffer, &buffer_size);
	EXPECT_EQ(result, 1);
	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(buffer_size, 0);
	EXPECT_STREQ(buffer,"This is a test\n");

	result = read_line(fh, &buffer, &buffer_size);
	EXPECT_EQ(result, 1);
	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(buffer_size, 0);
	EXPECT_STREQ(buffer,"file with 2 lines.");

	result = read_line(fh, &buffer, &buffer_size);
	EXPECT_EQ(result, 0);
	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(buffer_size, 0);

	free(buffer);
	fclose(fh);
}

TEST(TestReadLine, SmallUserDefinedBufferResizedFileRead)
{
	static char file_memory_buffer[] = "This is a test\nfile with 2 lines.";

	FILE *const fh = fmemopen(file_memory_buffer, sizeof(file_memory_buffer), "r");
	ASSERT_NE(fh, nullptr);

	size_t buffer_size = 10;
	char *buffer = (char *)malloc(buffer_size);
	int result = read_line(fh, &buffer, &buffer_size);
	EXPECT_EQ(result, 1);
	EXPECT_NE(buffer, nullptr);
	EXPECT_GT(buffer_size, 10);
	EXPECT_STREQ(buffer,"This is a test\n");

	const size_t buffer_size_previous = buffer_size;
	result = read_line(fh, &buffer, &buffer_size);
	EXPECT_EQ(result, 1);
	EXPECT_NE(buffer, nullptr);
	EXPECT_EQ(buffer_size, buffer_size_previous);
	EXPECT_STREQ(buffer,"file with 2 lines.");

	result = read_line(fh, &buffer, &buffer_size);
	EXPECT_EQ(result, 0);
	EXPECT_NE(buffer, nullptr);
	EXPECT_EQ(buffer_size, buffer_size_previous);

	free(buffer);
	fclose(fh);

}

#define LINE0 "A test.\n"
#define LINE1 "A paragraph should ideally be a smooth, succinct experience that goes through a bit of exposition, illustrates an idea, sums up the point, and primes the reader for the next paragraph.\n"
#define LINE2 "Ideally.\n"
#define LINE3 "\n"
#define LINE4 "In practice, a writer can get too caught up in all the things they have to say and fail to organize it all into bits an ordinary human being would be able to digest. The end result is a huge run-on paragraph that makes it difficult to recall the original point of it, if there was one in the first place. The reader's eyes glaze over and all they see is a wall of text.\n"
#define LINE5 "This afflicts all written media, but it is particularly infamous for its effect on Comic Books. One of the first things learned in comics is how to use dialogue bubbles effectively; a writer not allocating space carefully will end up covering their panel with a bunch of text and white space. Eventually the reader will realize that they're just looking at plain text rather than the vivid form of storytelling by imagery that comic books are famed for. TL;DR indeed.\n"
#define LINE6 "At best, a Wall of Text is just a signal of really heavy exposition. At worst, they are a warning sign that the author is soapboxing about something.\n"
#define LINE7 "Speaking in Panels is often a way to evade this trope while recounting what happened.\n"
#define LINE8 "If Speech-Bubbles Interruption are used to show it's not being listened to, see Wall of Blather. If the text is literally written on a wall in-universe, it might be a Room Full of Crazy. See Read the Fine Print if these kinds of text actually contain very important information. Ominous Multiple Screens is sort-of the video equivalent.\n"
#define LINE9 "For egregious examples right in This Very Wiki, observe the venerable entrants of Trope Overdosed.\n"
#define LINEA "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was born and I will give you a complete account of the system, and expound the actual teachings of the great explorer of the truth, the master-builder of human happiness. No one rejects, dislikes, or avoids pleasure itself, because it is pleasure, but because those who do not know how to pursue pleasure rationally encounter consequences that are extremely painful. Nor again is there anyone who loves or pursues or desires to obtain pain of itself, because it is pain, but because occasionally circumstances occur in which toil and pain can procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious physical exercise, except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure that has no annoying consequences, or one who avoids a pain that produces no resultant pleasure? On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matters to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains. But I must explain to you how\n"
#define NUM_LINES 11

static char *file_memory_buffer;
static const char *expected_lines[NUM_LINES] = {LINE0, LINE1, LINE2, LINE3, LINE4, LINE5, LINE6, LINE7, LINE8, LINE9, LINEA};

TEST(TestReadLine, LargeBuffer)
{
	size_t text_size = 0;
	for (int i = 0; i < NUM_LINES; ++i) {
		text_size += strlen(expected_lines[i]);
	}

	file_memory_buffer = (char *)malloc(text_size + 1);
	ASSERT_NE(file_memory_buffer, nullptr);

	size_t text_size_so_far = 0;
	for (int i = 0; i < NUM_LINES; ++i) {
		strcpy(file_memory_buffer + text_size_so_far, expected_lines[i]);
		text_size_so_far += strlen(expected_lines[i]);
	}
	EXPECT_EQ(text_size_so_far, text_size);

	FILE *const fh = fmemopen(file_memory_buffer, text_size + 1, "r");
	if (!fh)
		free(file_memory_buffer);
	ASSERT_NE(fh, nullptr);

	char *buffer = nullptr;
	size_t buffer_size;
	size_t max_line_size_so_far = 0;
	for (int i = 0; i < NUM_LINES; ++i) {
		SCOPED_TRACE(i);

		int result = read_line(fh, &buffer, &buffer_size);
		EXPECT_EQ(result, 1);
		EXPECT_NE(buffer, nullptr);
		if (max_line_size_so_far < buffer_size)
			max_line_size_so_far == buffer_size;
		EXPECT_GE(buffer_size, max_line_size_so_far);
		ASSERT_STREQ(buffer, expected_lines[i]);

		free(buffer);
		buffer = nullptr;
	}

	free(file_memory_buffer);
	fclose(fh);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}