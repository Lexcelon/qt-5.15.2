// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "storage/browser/file_system/memory_file_stream_writer.h"

#include <memory>
#include <utility>

#include "base/memory/ptr_util.h"
#include "base/task_runner_util.h"
#include "net/base/io_buffer.h"
#include "net/base/net_errors.h"

namespace storage {

std::unique_ptr<FileStreamWriter> FileStreamWriter::CreateForMemoryFile(
    scoped_refptr<base::TaskRunner> task_runner,
    base::WeakPtr<ObfuscatedFileUtilMemoryDelegate> memory_file_util,
    const base::FilePath& file_path,
    int64_t initial_offset) {
  return base::WrapUnique(new MemoryFileStreamWriter(
      std::move(task_runner), std::move(memory_file_util), file_path,
      initial_offset));
}

MemoryFileStreamWriter::MemoryFileStreamWriter(
    scoped_refptr<base::TaskRunner> task_runner,
    base::WeakPtr<ObfuscatedFileUtilMemoryDelegate> memory_file_util,
    const base::FilePath& file_path,
    int64_t initial_offset)
    : memory_file_util_(std::move(memory_file_util)),
      task_runner_(std::move(task_runner)),
      file_path_(file_path),
      offset_(initial_offset) {
  DCHECK(memory_file_util_.MaybeValid());
}

MemoryFileStreamWriter::~MemoryFileStreamWriter() = default;

int MemoryFileStreamWriter::Write(net::IOBuffer* buf,
                                  int buf_len,
                                  net::CompletionOnceCallback callback) {
  task_runner_->PostTaskAndReplyWithResult(
      FROM_HERE,
      base::BindOnce(
          [](base::WeakPtr<ObfuscatedFileUtilMemoryDelegate> util,
             const base::FilePath& path, int64_t offset,
             scoped_refptr<net::IOBuffer> buf, int buf_len) -> int {
            if (!util)
              return net::ERR_FILE_NOT_FOUND;
            base::File::Info file_info;
            if (util->GetFileInfo(path, &file_info) != base::File::FILE_OK)
              return net::ERR_FILE_NOT_FOUND;
            return util->WriteFile(path, offset, std::move(buf), buf_len);
          },
          memory_file_util_, file_path_, offset_, base::WrapRefCounted(buf),
          buf_len),
      base::BindOnce(&MemoryFileStreamWriter::OnWriteCompleted,
                     weak_factory_.GetWeakPtr(), std::move(callback)));

  return net::ERR_IO_PENDING;
}

void MemoryFileStreamWriter::OnWriteCompleted(
    net::CompletionOnceCallback callback,
    int result) {
  if (result > 0)
    offset_ += result;
  std::move(callback).Run(result);
}

int MemoryFileStreamWriter::Cancel(net::CompletionOnceCallback /*callback*/) {
  return net::ERR_UNEXPECTED;
}

int MemoryFileStreamWriter::Flush(net::CompletionOnceCallback /*callback*/) {
  return net::OK;
}
}  // namespace storage
